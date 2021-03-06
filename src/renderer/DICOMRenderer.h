#ifndef __DICOMRENDERER
#define __DICOMRENDERER

//used Trinity-renderer classes
#include <renderer/openGL/GLCore/GLVolumeBox.h>
#include <renderer/openGL/GLCore/GLBasicShapes.h>
#include <renderer/openGL/GLCore/GLModel.h>
#include <renderer/openGL/GLCore/GLBoundingBox.h>
#include <renderer/openGL/GLCore/GLRenderPlane.h>
#include <renderer/openGL/GLCore/GLRenderPlane.h>
#include <renderer/openGL/GLCore/GLFrameBufferObject.h>
#include <renderer/openGL/GLCore/GLProgram.h>
#include <renderer/openGL/GLCore/GLTexture3D.h>
#include <renderer/openGL/GLCore/GLTexture2D.h>
#include <renderer/openGL/GLCore/GLTexture1D.h>
#include <renderer/openGL/GLTargetBinder.h>
#include <renderer/tools/Camera.h>
#include <renderer/tools/ShaderDescriptor.h>

//core classes
#include <core/Math/Vectors.h>
#include <core/Time/Timer.h>

//brainvis classes
#include <io/DataHandle.h>
#include <io/Data/MERBundle.h>
#include <renderer/DICOMRendererEnums.h>

#include <memory>
#include <string>
#include <vector>

#include <renderer/fusion/iFusion.h>

using namespace Tuvok::Renderer;
using namespace Tuvok::Renderer::OpenGL;
using namespace Tuvok::Renderer::OpenGL::GLCore;
using namespace Core::Math;

static const Vec3f volumeOffset = Vec3f(0.5f,0.5f,0.5f);
static const Vec3f frameCenter = Vec3f(100.0f,100.0f,100.0f);

class DICOMRenderer{
    public:
        bool _displayFrameBoundingBox;
        bool _displayBoundingBox;
        bool _displayFrame;
        bool _displayCenterACPC;
        bool _displayThreeDCursor;
        bool _displayTwoDCursor;

    public:
        DICOMRenderer();
        virtual ~DICOMRenderer(){};

        //initializes the renderer and all members
        void Initialize();

        //used to cleanup and free memory
        void Cleanup();

        //draws the actual image using the current setup
        void Paint();

        //switches rendermode (3D, Axis slices etc)
        void SetRenderMode(const RenderMode& mode);
        RenderMode getRenderMode() const {return _activeRenderMode;}

        //resizes the framebuffer and all members which have to be recreated
        //after resize
        void SetWindowSize(uint32_t width, uint32_t height);

        //sets the shared_ptr to a data handle
        //data handles contain all shared options for a dataset used by
        //multiple renderer
        void SetDataHandle(const std::shared_ptr<DataHandle> dataHandle);

        //changes the currently viewed slide of the dicom volumes
        void ChangeSlide(int slidedelta);

        //zoom function for the 2D renderes \todo make one zoom function for 2d/3d
        void ZoomTwoD(int zoomDelta);

        //controlls the 3d camera
        void moveCamera(Vec3f dir);
        void setCameraLookAt(Vec3f lookAt);
        void rotateCamera(Vec3f rotation);

        //picking
        void PickPixel(Vec2ui coord);

        //will set a flag to start a complete repaint as
        //soon as possible
        void sheduleRepaint();

        //function to set the font image
        //the displayed font is currently drawn in a QImage
        //and is not part of the QTless renderer lib
        void setFontData(char* data);

        //set/get for active clipmode of a renderer
        DICOMClipMode clipMode() const;
        void setClipMode(const DICOMClipMode &clipMode);

        bool doesGradientDescent() const;
        void setDoesGradientDescent(bool doesGradientDescent, FusionMode mode);

        void findGFrame();
        void registerDatasets();

        void setDoFrameDetection(bool doFrameDetection);

        RenderMode activeRenderMode() const;

        void setIsTracking(bool isTracking);
        void updateTransferFunction();
private:

        //Loading stuff
        bool LoadShaderResources();
        bool LoadAndCheckShaders(std::shared_ptr<GLProgram>& programPtr, ShaderDescriptor& sd);
        bool LoadGeometry();
        bool LoadFrameBuffer();
        bool LoadFFTColorTex();

        void generateLeftFrameBoundingBox(Vec3f center, Vec3f scale);
        void generateRightFrameBoundingBox(Vec3f center, Vec3f scale);
        void createFrameGeometry(std::vector<Vec3f> corners, int id= 0);

        void checkDatasetStatus();
        void checkElectrodeStatus();
        void checkSettingStatus();

        //raycaster-------------------------------------------------------------------
        void RayCast();
        void drawCubeEntry(std::shared_ptr<GLFBOTex> target, Mat4f world);
        void drawVolumeRayCast(std::shared_ptr<GLFBOTex> colorTarget,
                           std::shared_ptr<GLFBOTex> positionTarget,
                           std::shared_ptr<GLProgram> shader,
                           Mat4f world,
                           GLuint entryHandle,
                           GLuint volumeHandle,
                           GLuint tfHandle,
                           float tfScaling,
                           bool isCT = true,
                           float rayStepPara = 1.0f);
        void drawLineBoxes(std::shared_ptr<GLFBOTex> target);

        void drawPlaning();
        void drawCenterCube(std::shared_ptr<GLFBOTex> target);
        void drawGFrame(std::shared_ptr<GLFBOTex> target);
        void drawElectrodeCylinder(std::shared_ptr<GLFBOTex> target);
        void drawElectrodeSpheres(std::shared_ptr<GLFBOTex> target);

        void drawCompositing();

        //slicer------------------------------------------------------------------------
        void SliceRendering();
        void drawSliceCompositing();
        void drawSlice(std::shared_ptr<GLProgram> shader, bool isCT = true,bool full = true, bool noCTBones = false);

        //G-Frame-------------------------------------------------------------------
        std::vector<Vec3f> findFrame(float startX = 0.0f, float stepX = 1.0f, Vec2f range = Vec2f(0.45f,0.6f), Vec3f minBox = Vec3f(0,0,0), Vec3f maxBox = Vec3f(1,1,1));
        std::vector<Vec3f> findFrameCorners(std::vector<Vec3f> data);
        void renderFramePosition();

        //utils
        void ClearBackground(Vec4f color);
        Core::Math::Mat4f calculateElectrodeMatices(Core::Math::Vec3f entry, Core::Math::Vec3f target);
        void checkForErrorCodes(std::string note);

    private:
        RenderMode                      _activeRenderMode;
        GLint                           _displayFramebufferID;

        //handle to the dataset informations----------------------
        std::shared_ptr<DataHandle>     _data;

        //textures used in the renderer---------------------------
        std::shared_ptr<GLTexture3D>    _GL_MRVolume;
        std::shared_ptr<GLTexture3D>    _GL_CTVolume;
        std::shared_ptr<GLTexture1D>    _transferFunctionTex;
        std::shared_ptr<GLTexture1D>    _FFTColor;
        std::shared_ptr<GLTexture2D>    _FontTexture;

        //shaders-------------------------------------------------
        std::shared_ptr<GLProgram>      _frontFaceShader;
        std::shared_ptr<GLProgram>      _NearPlanShader;
        std::shared_ptr<GLProgram>      _rayCastShader;
        std::shared_ptr<GLProgram>      _sliceShader;
        std::shared_ptr<GLProgram>      _compositingThreeDShader;
        std::shared_ptr<GLProgram>      _compositingTwoDShader;
        std::shared_ptr<GLProgram>      _lineShader;
        std::shared_ptr<GLProgram>      _sphereFFTShader;
        std::shared_ptr<GLProgram>      _frameSearchShader;
        std::shared_ptr<GLProgram>      _electrodeGeometryShader;
        std::shared_ptr<GLProgram>      _subtractRayCastShader;

        //geometry-----------------------------------------------
        std::unique_ptr<GLVolumeBox>    _volumeBox;
        std::unique_ptr<GLRenderPlane>  _renderPlane;
        std::unique_ptr<GLModel>        _electrodeGeometry;

        std::unique_ptr<GLBoundingBox>   _boundingBox;
        std::unique_ptr<GLSphere>        _sphere;
        std::unique_ptr<GLBoundingQuad>  _XAxisSlice;
        std::unique_ptr<GLBoundingQuad>  _YAxisSlice;
        std::unique_ptr<GLBoundingQuad>  _ZAxisSlice;
        std::unique_ptr<GLModel>         _NShape1;
        std::unique_ptr<GLModel>         _NShape2;
        std::unique_ptr<GLBoundingBox>   _cubeLeftN;
        std::unique_ptr<GLBoundingBox>   _cubeRightN;

        //framebuffer------------------------------------------
        std::unique_ptr<GLTargetBinder> _targetBinder;

        //3D View
        std::shared_ptr<GLFBOTex>       _rayEntryCT;
        std::shared_ptr<GLFBOTex>       _rayCastColorCT;
        std::shared_ptr<GLFBOTex>       _rayCastPositionCT;

        std::shared_ptr<GLFBOTex>       _rayEntryMR;
        std::shared_ptr<GLFBOTex>       _rayCastColorMR;
        std::shared_ptr<GLFBOTex>       _rayCastPositionMR;

        std::shared_ptr<GLFBOTex>       _boundingBoxVolumeBuffer;

        //projection/view matrices
        Vec2ui                          _windowSize;
        Mat4f                           _projection;
        Mat4f                           _view;

        std::unique_ptr<Camera>         _camera;
        bool                            _isTracking;

        bool                            _needsUpdate;
        uint64_t                        _datasetStatus;
        uint64_t                        _electrodeStatus;
        uint64_t                        _renderSettingStatus;

        DICOMClipMode                   _clipMode;

        //zoom for the slide renderer
        Vec3f                           _vZoom;


        //gradient decent testing
        bool                            _doesGradientDescent;
        bool                            _doFrameDetection;

        std::unique_ptr<iFusion>        _fusionMethod;
        FusionMode                      _fusionMode;
};


#endif

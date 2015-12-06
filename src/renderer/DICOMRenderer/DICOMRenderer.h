#ifndef __TNG__DICOMRENDERER__
#define __TNG__DICOMRENDERER__

//used Trinity-renderer classes
#include <renderer/ShaderDescriptor.h>
#include <renderer/OpenGL/GLCore/GLVolumeBox.h>
#include <renderer/OpenGL/GLCore/GLBasicShapes.h>
#include <renderer/OpenGL/GLCore/GLModel.h>
#include <renderer/OpenGL/GLCore/GLBoundingBox.h>
#include <renderer/OpenGL/GLCore/GLRenderPlane.h>
#include <renderer/OpenGL/GLCore/GLRenderPlane.h>
#include <renderer/OpenGL/GLCore/GLFrameBufferObject.h>
#include <renderer/OpenGL/GLCore/GLProgram.h>
#include <renderer/OpenGL/GLCore/GLTexture3D.h>
#include <renderer/OpenGL/GLCore/GLTexture2D.h>
#include <renderer/OpenGL/GLCore/GLTexture1D.h>
#include <renderer/OpenGL/GLTargetBinder.h>
#include <renderer/Camera.h>

//core classes
#include <core/Math/Vectors.h>
#include <core/Time/Timer.h>

//brainvis classes
#include <BrainVisIO/DataHandle.h>
#include <renderer/DICOMRenderer/DICOMRendererEnums.h>

#include <memory>
#include <string>
#include <vector>

using namespace Tuvok::Renderer;
using namespace Tuvok::Renderer::OpenGL;
using namespace Tuvok::Renderer::OpenGL::GLCore;
using namespace Core::Math;

class DICOMRenderer{
    public:
    enum RenderMode{
        ThreeDMode = 0,
        XAxis,
        YAxis,
        ZAxis
    };

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
        void SetRenderMode(RenderMode mode);

        //resizes the framebuffer and all members which have to be recreated
        //after resize
        void SetWindowSize(uint32_t width, uint32_t height);

        //sets the shared_ptr to a data handle
        //data handles contain all shared options for a dataset used by
        //multiple renderer
        void SetDataHandle(std::shared_ptr<DataHandle> dataHandle);

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

private:
        bool LoadShaderResources();
        bool LoadAndCheckShaders(std::shared_ptr<GLProgram>& programPtr, ShaderDescriptor& sd);
        bool LoadGeometry();
        bool LoadFrameBuffer();
        bool LoadFFTColorTex();

        void ClearBackground(Vec4f color);

        void updateTransferFunction();

        //raycaster------------------------------
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
                           float rayStepPara = 1000.0f);
        void drawLineBoxes(std::shared_ptr<GLFBOTex> target);

        void drawPlaning();
        void drawCenterCube(std::shared_ptr<GLFBOTex> target);
        void drawGFrame(std::shared_ptr<GLFBOTex> target);
        void drawElectrodeCylinder(std::shared_ptr<GLFBOTex> target);
        void drawElectrodeSpheres(std::shared_ptr<GLFBOTex> target);

        void drawCompositing();

        void checkDatasetStatus();

        //slicer--------------------------------
        void SliceRendering();
        void drawSliceCompositing();
        void drawSliceV3(bool isCT = true,bool full = true, bool noCTBones = false);

        //G-Frame-------------------------------
        bool _foundFrame;
        Vec3f                    _center;
        std::vector<Vec3f> findFrame(float startX = 0.0f, float stepX = 1.0f, Vec2f range = Vec2f(0.45f,0.6f));
        void frameSlicing(Vec2f range);
        void createFrameGeometry(std::vector<Vec3f> corners, int id= 0);

        void searchGFrame(Vec2f range = Vec2f(0.45f,0.6f));

        void calculateElectrodeMatices();
        void checkForErrorCodes(std::string note);

        float gradientDecentStep();
        float subVolumes(Vec2ui windowSize);

        void calculateRotation();

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
        std::shared_ptr<GLProgram>      _compositingVolumeSubstraction;
        std::shared_ptr<GLProgram>      _lineShader;
        std::shared_ptr<GLProgram>      _sphereFFTShader;
        std::shared_ptr<GLProgram>      _frameSearchShader;
        std::shared_ptr<GLProgram>      _electrodeGeometryShader;

        //geometry-----------------------------------------------
        std::unique_ptr<GLVolumeBox>    _volumeBox;
        std::unique_ptr<GLRenderPlane>  _renderPlane;
        std::unique_ptr<GLModel>        _electrodeGeometry;

        std::unique_ptr<GLRenderPlane>  _renderPlaneX;
        std::unique_ptr<GLRenderPlane>  _renderPlaneY;
        std::unique_ptr<GLRenderPlane>  _renderPlaneZ;

        std::unique_ptr<GLBoundingBox>   _boundingBox;
        std::unique_ptr<GLSphere>        _sphere;
        std::unique_ptr<GLBoundingQuad> _XAxisSlice;
        std::unique_ptr<GLBoundingQuad> _YAxisSlice;
        std::unique_ptr<GLBoundingQuad> _ZAxisSlice;
        std::unique_ptr<GLModel>         _NShape1;
        std::unique_ptr<GLModel>         _NShape2;

        //framebuffer------------------------------------------
        std::unique_ptr<GLTargetBinder> _targetBinder;

        //3D View
        std::shared_ptr<GLFBOTex>       _rayEntryCT;
        std::shared_ptr<GLFBOTex>       _rayCastColorCT;
        std::shared_ptr<GLFBOTex>       _rayCastPositionCT;

        std::shared_ptr<GLFBOTex>       _rayEntryMR;
        std::shared_ptr<GLFBOTex>       _rayCastColorMR;
        std::shared_ptr<GLFBOTex>       _rayCastPositionMR;

        //2D view
        std::shared_ptr<GLFBOTex>       _TwoDCTVolumeFBO;
        std::shared_ptr<GLFBOTex>       _TwoDCTPositionVolumeFBO;
        std::shared_ptr<GLFBOTex>       _TwoDMRVolumeFBO;
        std::shared_ptr<GLFBOTex>       _TwoDMRPositionVolumeFBO;
        std::shared_ptr<GLFBOTex>       _TwoDElectrodeFBO;
        std::shared_ptr<GLFBOTex>       _TwoDFontFBO;
        std::shared_ptr<GLFBOTex>       _TwoDTopFBO;
        std::shared_ptr<GLFBOTex>       _boundingBoxVolumeBuffer;

        //projection/view matrices
        Vec2ui                          _windowSize;
        Mat4f                           _projection;
        Mat4f                           _orthographicXAxis;
        Mat4f                           _view;
        Mat4f                           _viewX;

        std::unique_ptr<Camera>         _camera;

        bool                            _needsUpdate;
        uint64_t                        _datasetStatus;

        DICOMClipMode                   _clipMode;

        //matricies for the left and write electrode renderer (cylinder)
        Mat4f                           _electrodeLeftMatrix;
        Mat4f                           _electrodeRightMatix;

        //zoom for the slide renderer
        Vec3f                           _vZoom;


        //gradient decent testing
        std::shared_ptr<GLFBOTex>       CTFBO;
        std::shared_ptr<GLFBOTex>       MRFBO;
        std::shared_ptr<GLFBOTex>       COMPOSITING;
        float translationStepSize;
        float rotationStepSize;
        float scaleStepSize;

        Vec3f                           _ACMRWorldPosition;
        Vec3f                           _PCMRWorldPosition;
};


#endif

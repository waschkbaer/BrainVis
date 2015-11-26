#ifndef __TNG__DICOMRENDERER__
#define __TNG__DICOMRENDERER__

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

#include <core/Math/Vectors.h>
#include <core/Time/Timer.h>

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

        void Initialize();
        void Cleanup();

        void Resize();

        void Paint();

        void SetRenderMode(RenderMode mode);
        void SetWindowSize(uint32_t width, uint32_t height);
        void SetDataHandle(std::shared_ptr<DataHandle> dataHandle);
        void ChangeSlide(int slidedelta);
        void ZoomTwoD(int zoomDelta);

        void moveCamera(Vec3f dir);
        void setCameraLookAt(Vec3f lookAt);
        void rotateCamera(Vec3f rotation);
        void updateSlideView();

        void PickPixel(Vec2ui coord);
        void sheduleRepaint();

        void setFontData(char* data);

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

        //raycaster
        void RayCast();
        void drawCubeEntry(std::shared_ptr<GLFBOTex> target, Mat4f world);
        void drawVolumeRayCast(std::shared_ptr<GLFBOTex> colorTarget,
                           std::shared_ptr<GLFBOTex> positionTarget,
                           Mat4f world,
                           GLuint entryHandle,
                           GLuint volumeHandle,
                           GLuint tfHandle,
                           float tfScaling);
        void drawLineBoxes();
        void drawPlaning();
        void drawCompositing();

        void checkDatasetStatus();

        //slicer
        void SliceRendering();
        void drawSliceElectrode();
        void drawSliceTop();
        void drawSliceFont();
        void drawSliceCompositing();

        void drawSliceV2(GLuint volumeID,
                         float transferScaling,
                         std::shared_ptr<GLFBOTex> color,
                         std::shared_ptr<GLFBOTex> position,
                         Vec3f VolumeTranslation,
                         Vec3f VolumeScale,
                         bool secondary = false);

        //G-Frame
        bool _foundFrame;
        Vec3f                    _center;
        std::vector<Vec3f> findFrame(float startX = 0.0f, float stepX = 1.0f, Vec2f range = Vec2f(0.45f,0.6f));
        void frameSlicing(Vec2f range);
        void createFrameGeometry(std::vector<Vec3f> corners, int id= 0);
        void create2DGeometry();

        void searchGFrame(Vec2f range = Vec2f(0.45f,0.6f));

        void calculateElectrodeMatices();

    private:
        RenderMode                      _activeRenderMode;
        GLint                           _displayFramebufferID;

        std::shared_ptr<DataHandle>     _data;
        std::shared_ptr<GLTexture3D>    _GL_MRVolume;
        std::shared_ptr<GLTexture3D>    _GL_CTVolume;
        std::shared_ptr<GLTexture1D>    _transferFunctionTex;
        std::shared_ptr<GLTexture1D>    _FFTColor;
        std::shared_ptr<GLTexture2D>    _FontTexture;

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


        std::unique_ptr<GLTargetBinder> _targetBinder;
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

        Vec2ui                          _windowSize;
        Mat4f                           _projection;
        Mat4f                           _orthographicXAxis;
        Mat4f                           _orthographicYAxis;
        Mat4f                           _orthographicZAxis;
        Mat4f                           _orthonormalProjection;
        Mat4f                           _view;
        Mat4f                           _viewX;
        Mat4f                           _viewY;
        Mat4f                           _viewZ;

        Vec3f                           _lookAt;
        Vec3f                           _eyeDir;
        float                           _eyeDistance;

        Vec3f                           _vTranslation;

        Vec3f                           _vRotation;

        Core::Time::Timer               _timer;
        double                          _elapsedTime;

        std::unique_ptr<Camera>         _camera;

        bool                            _needsUpdate;
        uint64_t                        _datasetStatus;

        DICOMClipMode                   _clipMode;

        Mat4f                           _electrodeLeftMatrix;
        Mat4f                           _electrodeRightMatix;

        Vec3f                           _vXZoom;
        Vec3f                           _vYZoom;
        Vec3f                           _vZZoom;
};


#endif

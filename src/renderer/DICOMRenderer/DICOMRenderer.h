#ifndef __TNG__DICOMRENDERER__
#define __TNG__DICOMRENDERER__

#include <renderer/ShaderDescriptor.h>
#include <renderer/OpenGL/GLCore/GLVolumeBox.h>
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

#include <memory>
#include <string>
#include <vector>

using Tuvok::Renderer::OpenGL::GLCore::GLProgram;
using Tuvok::Renderer::OpenGL::GLCore::GLVolumeBox;
using Tuvok::Renderer::OpenGL::GLCore::GLBoundingBox;
using Tuvok::Renderer::OpenGL::GLCore::GLModel;
using Tuvok::Renderer::OpenGL::GLCore::GLSphere;
using Tuvok::Renderer::OpenGL::GLCore::GLBoundingQuad;
using Tuvok::Renderer::OpenGL::GLCore::GLBoundingQuadX;
using Tuvok::Renderer::OpenGL::GLCore::GLBoundingQuadY;
using Tuvok::Renderer::OpenGL::GLCore::GLBoundingQuadZ;
using Tuvok::Renderer::OpenGL::GLCore::GLRenderPlane;
using Tuvok::Renderer::OpenGL::GLCore::GLFBOTex;
using Tuvok::Renderer::OpenGL::GLCore::GLTexture3D;
using Tuvok::Renderer::OpenGL::GLCore::GLTexture2D;
using Tuvok::Renderer::OpenGL::GLCore::GLTexture1D;
using Tuvok::Renderer::Camera;
using Tuvok::Renderer::ShaderDescriptor;
using Tuvok::Renderer::OpenGL::GLTargetBinder;
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

        void moveCamera(Vec3f dir);
        void rotateCamera(Vec3f rotation);
        void updateSlideView();

        void PickPixel(Vec2ui coord);
        void sheduleRepaint();

        void setFontData(char* data);

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
        void drawSliceVolume(GLuint volumeID,
                             float transferScaling,
                             Vec3f volumeScale,
                             std::shared_ptr<GLFBOTex> color,
                             std::shared_ptr<GLFBOTex> position,
                             Vec3f currentSlices,
                             Vec3f trans,
                             bool ct = true);
        void drawSliceElectrode();
        void drawSliceTop();
        void drawSliceFont();
        void drawSliceCompositing();

        //G-Frame
        bool _foundFrame;
        Vec3f                    _center;
        std::vector<Vec3f> findFrame(float startX = 0.0f, float stepX = 1.0f, Vec2f range = Vec2f(0.45f,0.6f));
        void frameSlicing(Vec2f range);
        void createFrameGeometry(std::vector<Vec3f> corners, int id= 0);
        void create2DGeometry();

        void searchGFrame(Vec2f range = Vec2f(0.45f,0.6f));

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


        std::unique_ptr<GLTargetBinder> _targetBinder;
        std::unique_ptr<GLVolumeBox>    _volumeBox;
        std::unique_ptr<GLRenderPlane>  _renderPlane;

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

        std::shared_ptr<GLFBOTex>       _rayEntryCT;
        std::shared_ptr<GLFBOTex>       _rayCastColorCT;
        std::shared_ptr<GLFBOTex>       _rayCastPositionCT;

        std::shared_ptr<GLFBOTex>       _rayEntryMR;
        std::shared_ptr<GLFBOTex>       _rayCastColorMR;
        std::shared_ptr<GLFBOTex>       _rayCastPositionMR;

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
};


#endif

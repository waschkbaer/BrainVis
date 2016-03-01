#ifndef OPENGLFUSION
#define OPENGLFUSION

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

using namespace Tuvok::Renderer;
using namespace Tuvok::Renderer::OpenGL;
using namespace Tuvok::Renderer::OpenGL::GLCore;
using namespace Core::Math;

#include <vector>

#include "iFusion.h"

class OpenGLFusion: public iFusion{
public:
    OpenGLFusion(std::shared_ptr<DataHandle> d, Vec2ui WindowResolution, uint32_t CTTextureHandle, uint32_t MRTextureHandle);
    ~OpenGLFusion();

    int32_t executeFusionStep();
    bool init(Core::Math::Vec2ui resolution = Vec2ui(0,0));
private:
    std::vector<float> doGD();

private:
    Vec2ui                          _windowSize;
    Vec2ui                          _windowSizeReal;
    std::vector<Core::Math::Mat4f>  _matrices;
    uint32_t                        _ctHandle;
    uint32_t                        _mrHandle;

    std::shared_ptr<GLProgram>      _subtractRayCastShader;
    std::shared_ptr<GLProgram>      _frontFaceShader;

    std::unique_ptr<GLVolumeBox>    _volumeBox;
    std::unique_ptr<GLTargetBinder> _targetBinder;

    std::shared_ptr<GLFBOTex>       _rayEntryBuffer;
    std::shared_ptr<GLFBOTex>       _raySubBuffer1;
    std::shared_ptr<GLFBOTex>       _raySubBuffer2;
    std::shared_ptr<GLFBOTex>       _raySubBuffer3;
    std::shared_ptr<GLFBOTex>       _raySubBuffer4;


};


#endif //OPENGLFUSION

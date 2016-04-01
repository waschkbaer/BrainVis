#include "DICOMRenderer.h"
#include <renderer/openGL/OpenGLDefines.h>
#include <algorithm>
#include <renderer/tools/ContextMutex.h>
#include <cstdlib>


#include <io/Data/MERBundleManager.h>
#include <io/Data/MERBundle.h>
#include <io/Data/MERElectrode.h>
#include <io/Data/MERData.h>

#include "DICOMRenderManager.h"

#include <renderer/Fusion/OpenGLBasedFusion.h>
#include <renderer/Fusion/CPUBasedFusion.h>
#include <renderer/Fusion/CudaFusion.h>
#define SUPPORTS_CUDA

#ifdef SUPPORTS_CUDA
#include <renderer/Fusion/CudaKernel.h>
#endif
#include <core/Time/Timer.h>


using namespace std;
using namespace Tuvok::Renderer;

static Core::Time::Timer timer;

static int screenshot(int i)
{
    unsigned char *pixels;
    FILE *image;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    pixels = new unsigned char[viewport[2] * viewport[3] * 3];

    glReadPixels(0, 0, viewport[2], viewport[3], GL_RGB,
        GL_UNSIGNED_BYTE, pixels);

    char tempstring[50];
    sprintf(tempstring, "screenshot_%i.tga", i);
    if ((image = fopen(tempstring, "wb")) == NULL) return 1;

    unsigned char TGAheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    unsigned char header[6] = { static_cast<unsigned char>(((int)(viewport[2] % 256))),
        static_cast<unsigned char>(((int)(viewport[2] / 256))),
        static_cast<unsigned char>(((int)(viewport[3] % 256))),
        static_cast<unsigned char>(((int)(viewport[3] / 256))), 24, 0 };

    // TGA header schreiben
    fwrite(TGAheader, sizeof(unsigned char), 12, image);
    // Header schreiben
    fwrite(header, sizeof(unsigned char), 6, image);

    fwrite(pixels, sizeof(unsigned char),
        viewport[2] * viewport[3] * 3, image);

    fclose(image);
    delete[] pixels;

    return 0;
}


DICOMRenderer::DICOMRenderer():
_activeRenderMode(ThreeDMode),
_windowSize(512,512),
_GL_CTVolume(nullptr),
_GL_MRVolume(nullptr),
_camera(new Camera(Vec3f(0,-400,0),Vec3f(0,0,0),Vec3f(0,0,1))),
_datasetStatus(0),
_electrodeStatus(0),
_clipMode(DICOMClipMode::none),
_electrodeGeometry(nullptr),
_vZoom(1,1,1),
_needsUpdate(true),
_doesGradientDescent(false),
_doFrameDetection(false),
_displayFrameBoundingBox(true),
_displayBoundingBox(true),
_displayFrame(true),
_displayCenterACPC(true),
_displayThreeDCursor(true),
_displayTwoDCursor(true),
_isTracking(true),
_fusionMethod(nullptr),
_renderSettingStatus(0),
  _fusionMode(FusionMode::OpenGL)
{
}

void DICOMRenderer::Initialize(){
    //loading shaders, geometry and framebuffers
    if(!LoadShaderResources()) cout << "error in load shaders" << endl;
    checkForErrorCodes("@ after Load Shader");
    if(!LoadGeometry()) cout << "error in load geometry" << endl;
    checkForErrorCodes("@ after Load Geometry");
    if(!LoadFrameBuffer()) cout << "error in load framebuffer" << endl;
    checkForErrorCodes("@ after Load FrameBuffer");

    //creating a targetBinder to simplify switching between framebuffers
    _targetBinder = std::unique_ptr<GLTargetBinder>(new GLTargetBinder());

    sheduleRepaint();
}

void DICOMRenderer::Cleanup(){
    _GL_MRVolume->Delete();
    _GL_CTVolume->Delete();
    _transferFunctionTex->Delete();
    _FFTColor->Delete();
    _FontTexture->Delete();

    _volumeBox = nullptr;
    _renderPlane = nullptr;
    _electrodeGeometry = nullptr;
    _boundingBox = nullptr;
    _sphere = nullptr;
    _XAxisSlice = nullptr;
    _YAxisSlice = nullptr;
    _ZAxisSlice = nullptr;
    _NShape1 = nullptr;
    _NShape2 = nullptr;
    _cubeLeftN = nullptr;
    _cubeRightN = nullptr;

    _targetBinder = nullptr;
}


void DICOMRenderer::SetRenderMode(const RenderMode& mode){
    _activeRenderMode = mode;
    sheduleRepaint();
}

void DICOMRenderer::SetWindowSize(uint32_t width, uint32_t height){
    //check if the size of the screen is changed
    if(width != _windowSize.x || height != _windowSize.y){
        _windowSize.x = width;
        _windowSize.y = height;

        //update projection and viewport
        glViewport(0,0,width,height);

        //recreate the framebuffers
        LoadFrameBuffer();

        sheduleRepaint();
    }
}

void DICOMRenderer::SetDataHandle(const std::shared_ptr<DataHandle> dataHandle){

    //set the shared ptr
    _data = dataHandle;

    // create the ct texture
    if(_data->getCTLoaded()){
        _GL_CTVolume = std::make_shared<GLTexture3D>(_data->getCTDimensions().x,
                                                     _data->getCTDimensions().y,
                                                     _data->getCTDimensions().z,
                                                     GL_R16,GL_RED,GL_UNSIGNED_SHORT,
                                                     &(_data->getCTData()[0]),
                                                     GL_LINEAR,GL_LINEAR);
    }
    // create the MRI texture
    if(_data->getMRLoaded()){
        _GL_MRVolume = std::make_shared<GLTexture3D>(_data->getMRDimensions().x,
                                                     _data->getMRDimensions().y,
                                                     _data->getMRDimensions().z,
                                                     GL_R16,GL_RED,GL_UNSIGNED_SHORT,
                                                     &(_data->getMRData()[0]),
                                                     GL_LINEAR,GL_LINEAR);
    }

    // update the Transferfunction
    _transferFunctionTex= nullptr;
    updateTransferFunction();

    // try to read the QT framebuffer id, it is != 0
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );

    // creates a 1D Texture with a specified colorrange for
    // the filtered dB of the FFT
    if(!LoadFFTColorTex()) cout << "error in ffttex"<< endl;

    _data->setSelectedSlices(Vec3f(0.5f,0.5f,0.5f));
#ifdef SUPPORTS_CUDA
    unsigned short *ctPtr = &((unsigned short)_data->getCTData()[0]);
    unsigned short *mrPtr = &((unsigned short)_data->getMRData()[0]);
    generateCudaTexture(mrPtr,
                        _data->getMRDimensions().x,
                        _data->getMRDimensions().y,
                        _data->getMRDimensions().z,
                        false);
    generateCudaTexture(ctPtr,
                        _data->getCTDimensions().x,
                        _data->getCTDimensions().y,
                        _data->getCTDimensions().z,
                        true);
#endif
    sheduleRepaint();
}

void DICOMRenderer::ChangeSlide(int slidedelta){
    Vec3f currentSlice = _data->getSelectedSlices();
    switch(_activeRenderMode){
    case RenderMode::ThreeDMode:
                                _camera->Zoom(4.1f*(float)slidedelta);
                                _view = _camera->buildLookAt();
                                break;
        case RenderMode::ZAxis :
                                currentSlice.z +=slidedelta* (1.0f/(float)_data->getCTDimensions().z)/2.0f;
                                _data->setSelectedSlices(currentSlice);
                                break;
        case RenderMode::YAxis :
                                currentSlice.y +=slidedelta* (1.0f/(float)_data->getCTDimensions().y)/2.0f;
                                _data->setSelectedSlices(currentSlice);
                                 break;
        case RenderMode::XAxis :
                                currentSlice.x +=slidedelta* (1.0f/(float)_data->getCTDimensions().x)/2.0f;
                                _data->setSelectedSlices(currentSlice);
                                break;
    }
   sheduleRepaint();
}

void DICOMRenderer::ZoomTwoD(int zoomDelta){

    float z = zoomDelta*0.2f;
    switch(_activeRenderMode){
    case RenderMode::ThreeDMode:
                                _camera->Zoom(4.1f*(float)zoomDelta);
                                _view = _camera->buildLookAt();
                                break;
        case RenderMode::ZAxis :
                                _vZoom.z += z*0.1f;
                                _vZoom.z = std::max(0.1f,_vZoom.z);
                                break;
        case RenderMode::YAxis :
                               _vZoom.y += z*0.1f;
                               _vZoom.y = std::max(0.1f,_vZoom.y);
                                 break;
        case RenderMode::XAxis :
                                _vZoom.x += z*0.1f;
                                _vZoom.x = std::max(0.1f,_vZoom.x);
                                break;
    }
    sheduleRepaint();
}

void DICOMRenderer::moveCamera(Vec3f dir)
{
    _camera->moveCamera(dir);
    _view = _camera->buildLookAt();
    sheduleRepaint();
}

void DICOMRenderer::setCameraLookAt(Vec3f lookAt){
    _camera->setLookAt(lookAt);
    _view = _camera->buildLookAt();
    sheduleRepaint();
}

void DICOMRenderer::rotateCamera(Vec3f rotation)
{
    //change rotate camera
    _camera->rotateCamera(rotation);
    _view = _camera->buildLookAt();
    sheduleRepaint();
}

void DICOMRenderer::sheduleRepaint(){
    _needsUpdate = true;
}

void DICOMRenderer::setFontData(char* data){
    _FontTexture->SetData((void*) data);
}

void DICOMRenderer::checkDatasetStatus(){
    if(_datasetStatus < _data->getDataSetStatus() ){
        sheduleRepaint();
        _datasetStatus = _data->getDataSetStatus();
    }
}

void DICOMRenderer::checkElectrodeStatus(){
    if(_electrodeStatus < BrainVisIO::MERData::MERBundleManager::getInstance().getBundleStatus()){
        sheduleRepaint();
        _electrodeStatus = BrainVisIO::MERData::MERBundleManager::getInstance().getBundleStatus();
    }
}

void DICOMRenderer::checkSettingStatus(){
    if(DicomRenderManager::getInstance().getRenderSettingStatus() > _renderSettingStatus){
        updateTransferFunction();
        _renderSettingStatus = DicomRenderManager::getInstance().getRenderSettingStatus();
        sheduleRepaint();
    }
}

void DICOMRenderer::Paint(){
    Tuvok::Renderer::Context::ContextMutex::getInstance().lockContext();
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );

    checkDatasetStatus();
    checkElectrodeStatus();
    checkSettingStatus();

    findGFrame();
    registerDatasets();

    glBindFramebuffer(GL_FRAMEBUFFER, _displayFramebufferID);
    //QT SUCKS and forces us to seperate into an else branch
    if(_needsUpdate){

        //check projectiontype
        if(!DicomRenderManager::getInstance().getOrthonormalThreeD()){
            _projection.Perspective(45, (float)_windowSize.x/(float)_windowSize.y,0.01f, 1000.0f);
        }else{
            _projection.Ortho(-(int32_t)_windowSize.x/2*_vZoom.x,(int32_t)_windowSize.x/2*_vZoom.x,-(int32_t)_windowSize.y/2*_vZoom.x,(int32_t)_windowSize.y/2*_vZoom.x,-5000.0f,5000.0f);
        }

        //check if tracking enabled
        if(_isTracking)
            setCameraLookAt(_data->getSelectedWorldSpacePositon());

        //check if the frame detection boxes are enabled
        if(DicomRenderManager::getInstance().getDisplayFrameDetectionBox()){
            generateLeftFrameBoundingBox(_data->getLeftFBBCenter()-volumeOffset,_data->getLeftFBBScale());
            generateRightFrameBoundingBox(_data->getRightFBBCenter()-volumeOffset,_data->getRightFBBScale());
        }
        // this has to be done if the frame was found
        if(_data->getBFoundCTFrame()){
            createFrameGeometry(_data->getLeftMarker(),0);
            createFrameGeometry(_data->getRightMarker(),1);
        }

        if(_activeRenderMode == ThreeDMode){
            RayCast();
        }else{
            SliceRendering();
        }
        _needsUpdate = false;


    //we have to to the compositing on each frame, qt5.5
    //has problems with not switching the framebuffer =/
    }else{

         if(_activeRenderMode == ThreeDMode){
             drawCompositing();
         }else{
             drawSliceCompositing();
         }
    }
    Tuvok::Renderer::Context::ContextMutex::getInstance().unlockContext();
}


bool DICOMRenderer::LoadShaderResources(){
    checkForErrorCodes("@ start Load Shader - clean error");
    vector<string> fs,vs;
    vs.push_back("Shader/CubeVertex.glsl");
    fs.push_back("Shader/CubeFragment.glsl");
    ShaderDescriptor sd(vs,fs);
    if(!LoadAndCheckShaders(_frontFaceShader,sd)) return false;
    checkForErrorCodes("@ Load CubeShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/NearPlaneVertex.glsl");
    fs.push_back("Shader/NearPlaneFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_NearPlanShader,sd)) return false;
    checkForErrorCodes("@ Load NearPlaneShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/RayCasterVertex.glsl");
    fs.push_back("Shader/RayCasterFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_rayCastShader,sd)) return false;
    checkForErrorCodes("@ Load RayCastShader");


    vs.clear();
    fs.clear();
    vs.push_back("Shader/RayCasterVertex.glsl");
    fs.push_back("Shader/RayCasterSubtractFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_subtractRayCastShader,sd)) return false;
    checkForErrorCodes("@ Load RayCastSubtractShader");


    vs.clear();
    fs.clear();
    vs.push_back("Shader/ComposeVertex.glsl");
    fs.push_back("Shader/ComposeFragment_ThreeD.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_compositingThreeDShader,sd)) return false;
    checkForErrorCodes("@ Load Compositing3DShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/ComposeVertex.glsl");
    fs.push_back("Shader/ComposeFragment_TwoD.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_compositingTwoDShader,sd)) return false;
    checkForErrorCodes("@ Load Compositing2DShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/RayCasterVertex.glsl");
    fs.push_back("Shader/SliceFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_sliceShader,sd)) return false;
    checkForErrorCodes("@ Load SliceShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/LineVertex.glsl");
    fs.push_back("Shader/LineFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_lineShader,sd)) return false;
    checkForErrorCodes("@ Load LineShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/FFTColorVertex.glsl");
    fs.push_back("Shader/FFTColorFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_sphereFFTShader,sd)) return false;
    checkForErrorCodes("@ Load SphereShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/RayCasterVertex.glsl");
    fs.push_back("Shader/FrameFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_frameSearchShader,sd)) return false;
    checkForErrorCodes("@ Load GFrameShader");

    vs.clear();
    fs.clear();
    vs.push_back("Shader/CubeVertex.glsl");
    fs.push_back("Shader/ElectrodeGeometryFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_electrodeGeometryShader,sd)) return false;
    checkForErrorCodes("@ Load ElectrodeShader");

    return true;
}


bool DICOMRenderer::LoadAndCheckShaders(std::shared_ptr<GLProgram>& programPtr, ShaderDescriptor& sd){
    programPtr = std::make_shared<GLProgram>();
    programPtr->Load(sd);

    if (!programPtr->IsValid()){
        cout << "programm not valid" << endl;
        return false;
    }
    return true;
}


bool DICOMRenderer::LoadGeometry(){
    checkForErrorCodes("@start LoadGeometry - clean error");
    _volumeBox = std::unique_ptr<GLVolumeBox>(new GLVolumeBox());
    _renderPlane = std::unique_ptr<GLRenderPlane>(new GLRenderPlane(_windowSize));
    _boundingBox = std::unique_ptr<GLBoundingBox>(new GLBoundingBox());
    _XAxisSlice = std::unique_ptr<GLBoundingQuadX>(new GLBoundingQuadX());
    _YAxisSlice = std::unique_ptr<GLBoundingQuadY>(new GLBoundingQuadY());
    _ZAxisSlice = std::unique_ptr<GLBoundingQuadZ>(new GLBoundingQuadZ());
    _sphere = std::unique_ptr<GLSphere>(new GLSphere());

    _electrodeGeometry = std::unique_ptr<GLCylinder>(new GLCylinder(0.5f,20));

    return true;
}


bool DICOMRenderer::LoadFrameBuffer(){
     checkForErrorCodes("@start LoadFrameBuffer - clean error");
     _rayEntryCT                    = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastColorCT                = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastPositionCT             = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayEntryMR                    = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastColorMR                = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastPositionMR             = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _boundingBoxVolumeBuffer       = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);

     //fontbuffer
     _FontTexture                   = std::make_shared<GLTexture2D>(_windowSize.x, _windowSize.y, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
     checkForErrorCodes("@end LoadFrameBuffer - clean error");
     return true;
 }

void DICOMRenderer::checkForErrorCodes(std::string note){
    uint32_t errorCode = glGetError();
    if(errorCode != GL_NO_ERROR){
        std::cout << "ERROR @ note:"<< note.c_str() <<std::endl;
        switch(errorCode){
            case GL_INVALID_ENUM : std::cout << "INVALID ENUM"<<std::endl;break;
            case GL_INVALID_VALUE : std::cout << "INVALID VALUE"<<std::endl;break;
            case GL_INVALID_OPERATION : std::cout << "INVALID OPERATION"<<std::endl;break;
            case GL_INVALID_FRAMEBUFFER_OPERATION : std::cout << "INVALID FRAMEBUFFER OPERATION"<<std::endl;break;
            case GL_OUT_OF_MEMORY : std::cout << "OUT OF MEMORY"<<std::endl;break;
            default : std::cout << "UNKNOWN ERRORCODE"<<std::endl;
        }
    }
}
void DICOMRenderer::setIsTracking(bool isTracking)
{
    _isTracking = isTracking;
}

RenderMode DICOMRenderer::activeRenderMode() const
{
    return _activeRenderMode;
}

void DICOMRenderer::setDoFrameDetection(bool doFrameDetection)
{
    _doFrameDetection = doFrameDetection;
}

bool DICOMRenderer::LoadFFTColorTex(){
    std::vector<Core::Math::Vec3f> color = _data->getFFTColorImage();
    _FFTColor = std::make_shared<GLTexture1D>(color.size(),GL_RGB32F,GL_RGB,GL_FLOAT,&(color[0]));
    return true;
}


void DICOMRenderer::ClearBackground(Vec4f color){
    glClearColor(color.x,color.y,color.z,color.w);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------3D-Rendering-Start--------------------------------------------------------------
void DICOMRenderer::RayCast(){
    //get the default framebuffer (QT Sucks hard!)
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );
    // && DicomRenderManager::getInstance().getBlendValue() > 0.0f
    if(_GL_CTVolume != nullptr){
        drawCubeEntry(_rayEntryCT,_data->getCTWorld());

        drawVolumeRayCast(  _rayCastColorCT,
                            _rayCastPositionCT,
                            _rayCastShader,
                            _data->getCTWorld(),
                            _rayEntryCT->GetTextureHandle(),
                            _GL_CTVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getCTTransferScaling(),
                            true,
                            DicomRenderManager::getInstance().getPerformanceValue());
    }
    // && DicomRenderManager::getInstance().getBlendValue() < 1.0f
    if(_GL_MRVolume != nullptr){
        drawCubeEntry(_rayEntryMR,_data->getMRWorld());

        drawVolumeRayCast(  _rayCastColorMR,
                            _rayCastPositionMR,
                            _rayCastShader,
                            _data->getMRWorld(),
                            _rayEntryMR->GetTextureHandle(),
                            _GL_MRVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getMRTransferScaling(),
                            false,
                            DicomRenderManager::getInstance().getPerformanceValue());
    }

    drawLineBoxes(_boundingBoxVolumeBuffer);

    drawPlaning();

    drawCompositing();
}


void DICOMRenderer::drawCubeEntry(std::shared_ptr<GLFBOTex> target, Mat4f world){
    //------- Raycaster Rendering
    _targetBinder->Bind(target);

    ClearBackground(Vec4f(0,0,0,0));

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    _NearPlanShader->Enable();
    _NearPlanShader->Set("viewMatrixInverse", _view.inverse());
    _NearPlanShader->Set("projectionMatrixInverse", _projection.inverse());
    _NearPlanShader->Set("worldMatrixInverse", world.inverse());

    _renderPlane->paint();

    _NearPlanShader->Disable();

    _frontFaceShader->Enable();
    _frontFaceShader->Set("projectionMatrix",_projection);
    _frontFaceShader->Set("viewMatrix",_view);
    _frontFaceShader->Set("worldMatrix",world);

    _volumeBox->paint();

    _frontFaceShader->Disable();
    _targetBinder->Unbind();
}


void DICOMRenderer::drawVolumeRayCast(  std::shared_ptr<GLFBOTex> colorTarget,
                                        std::shared_ptr<GLFBOTex> positionTarget,
                                        std::shared_ptr<GLProgram> shader,
                                        Mat4f world,
                                        GLuint entryHandle,
                                        GLuint volumeHandle,
                                        GLuint tfHandle,
                                        float tfScaling,
                                        bool isCT,
                                        float rayStepPara){
    _targetBinder->Bind(colorTarget,positionTarget);
    ClearBackground(Vec4f(0,0,0,-1000.0f));


    glCullFace(GL_FRONT);
    shader->Enable();
    shader->Set("projectionMatrix",_projection);
    shader->Set("viewMatrix",_view);
    shader->Set("worldMatrix",world);
    shader->SetTexture2D("rayStartPoint",entryHandle,0);
    shader->SetTexture3D("volume",volumeHandle,1);
    shader->SetTexture1D("transferfunction",tfHandle,2);
    shader->Set("worldFragmentMatrix",world);
    shader->Set("viewFragmentMatrix",_view);
    shader->Set("tfScaling",tfScaling);
    shader->Set("eyePos",_camera->GetWorldPosition());
    shader->Set("focusWorldPos",_data->getSelectedWorldSpacePositon());
    shader->Set("stepSize",rayStepPara);
    shader->Set("MRIValue",(float)_data->getMriValue());
    shader->Set("MRIPosition",_data->getMriVolumePosition());


    if(isCT){
        shader->Set("isCTImage",1.0f);
    }else{
        shader->Set("isCTImage",0.0f);
    }

    switch(_clipMode){
    case DICOMClipMode::none :      shader->Set("cutMode",0); break;
    case DICOMClipMode::CubicCut :  shader->Set("cutMode",1); break;

    case DICOMClipMode::PlaneX :
                                    shader->Set("cutPlaneNormal",_data->getCTeX());

                                    if(_activeRenderMode == RenderMode::XAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;

    case DICOMClipMode::PlaneY :
                                    shader->Set("cutPlaneNormal",_data->getCTeY());

                                    if(_activeRenderMode == RenderMode::YAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;

    case DICOMClipMode::PlaneZ :
                                    shader->Set("cutPlaneNormal",_data->getCTeZ());

                                    if(_activeRenderMode == RenderMode::ZAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;
    case DICOMClipMode::PlaneXn :
                                    shader->Set("cutPlaneNormal",-_data->getCTeX());

                                    if(_activeRenderMode == RenderMode::XAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;

    case DICOMClipMode::PlaneYn :
                                    shader->Set("cutPlaneNormal",-_data->getCTeY());

                                    if(_activeRenderMode == RenderMode::YAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;

    case DICOMClipMode::PlaneZn :
                                    shader->Set("cutPlaneNormal",-_data->getCTeZ());

                                    if(_activeRenderMode == RenderMode::ZAxis){
                                        shader->Set("cutMode",3);
                                    }else{
                                        shader->Set("cutMode",2);
                                    }

                                    break;
    default:                        shader->Set("cutMode",0); break;
    }

    _volumeBox->paint();

    shader->Disable();

    _targetBinder->Unbind();
}



void DICOMRenderer::drawLineBoxes(std::shared_ptr<GLFBOTex> target){

    //------- Bounding Box for CT Volume
    _targetBinder->Bind(target);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    ClearBackground(Vec4f(0,0,0,-800.0f));
    _lineShader->Enable();

    if(DicomRenderManager::getInstance().getDisplayBoundingBox()){

        _lineShader->Set("projectionMatrix",_projection);
        _lineShader->Set("viewMatrix",_view);
        _lineShader->Set("worldMatrix",_data->getCTWorld());
        _lineShader->Set("color",Vec3f(1,1,1));

        _boundingBox->paint(GL_LINES);


        //this part draws the planes which indicate the current CT slide
        Mat4f translation;
        Mat4f sliceWorld;
        Vec3f currentSlices = _data->getSelectedSlices();

        translation.Translation(currentSlices.x-0.5f,0,0);
        sliceWorld = translation*_data->getCTWorld();
        _lineShader->Set("worldMatrix",sliceWorld);
        _lineShader->Set("color",Vec3f(1,1,1));
        _XAxisSlice->paint(GL_LINES);

        translation.Translation(0,currentSlices.y-0.5f,0);
        sliceWorld = translation*_data->getCTWorld();
        _lineShader->Set("worldMatrix",sliceWorld);
        _lineShader->Set("color",Vec3f(1,1,1));
        _YAxisSlice->paint(GL_LINES);

        translation.Translation(0,0,currentSlices.z-0.5f);
        sliceWorld = translation*_data->getCTWorld();
        _lineShader->Set("worldMatrix",sliceWorld);
        _lineShader->Set("color",Vec3f(1,1,1));
        _ZAxisSlice->paint(GL_LINES);
    }
    _lineShader->Disable();
    _targetBinder->Unbind();

}


void DICOMRenderer::drawPlaning(){
    //we draw the electrode/g frame in the same FBO as the bounding box
    //could be changed later

    drawGFrame(_boundingBoxVolumeBuffer);

    drawCenterCube(_boundingBoxVolumeBuffer);

    if(DicomRenderManager::getInstance().getDisplayElectrodes()){
        drawElectrodeCylinder(_boundingBoxVolumeBuffer);
        drawElectrodeSpheres(_boundingBoxVolumeBuffer);
    }
}


void DICOMRenderer::drawCenterCube(std::shared_ptr<GLFBOTex> target){
    if(_displayCenterACPC){
        _targetBinder->Bind(target);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);

        //draws a cube in the center (coord 100,100,100 in gframe)
        Mat4f scaleT;
        scaleT.Scaling(1.0,1.0,1.0);
        Mat4f transT;
        transT.Translation(0,0,0);
        transT = scaleT*transT;

        _frontFaceShader->Enable();
        _frontFaceShader->Set("projectionMatrix",_projection);
        _frontFaceShader->Set("viewMatrix",_view);
        transT.Translation(_data->getCTCenter()*_data->getCTScale());
        transT = scaleT*transT;

        _frontFaceShader->Set("worldMatrix",transT);
        _volumeBox->paint();

        Vec3f AC = _data->getAC();
        Vec3f PC = _data->getPC();

        AC = AC-frameCenter;
        PC = PC-frameCenter;

        AC = AC.x*_data->getCTeX()+
                AC.y*_data->getCTeY()+
                AC.z*_data->getCTeZ()+
                _data->getCTCenter()*_data->getCTScale();
        PC = PC.x*_data->getCTeX()+
                PC.y*_data->getCTeY()+
                PC.z*_data->getCTeZ()+
                _data->getCTCenter()*_data->getCTScale();


        transT.Translation(AC);
        transT = scaleT*transT;

        _frontFaceShader->Set("worldMatrix",transT);
        _sphere->paint();

        transT.Translation(PC);
        transT = scaleT*transT;

        _frontFaceShader->Set("worldMatrix",transT);
        _sphere->paint();

        _frontFaceShader->Disable();

        _targetBinder->Unbind();
    }
}


void DICOMRenderer::drawGFrame(std::shared_ptr<GLFBOTex> target){
    _targetBinder->Bind(target);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    _lineShader->Enable();

    if(DicomRenderManager::getInstance().getDisplayFrameShapes()){

    //check if we found the G Frame and draws it if found-------------

        if(_NShape1 != nullptr){

            _lineShader->Set("projectionMatrix",_projection);
            _lineShader->Set("viewMatrix",_view);
            _lineShader->Set("worldMatrix",_data->getCTWorld());
            _lineShader->Set("color",Vec3f(0.5f,1,0.5f));

            _NShape1->paint(GL_LINES);

        }

        if(_NShape2 != nullptr){

            _lineShader->Set("projectionMatrix",_projection);
            _lineShader->Set("viewMatrix",_view);
            _lineShader->Set("worldMatrix",_data->getCTWorld());
            _lineShader->Set("color",Vec3f(0.5f,1,0.5f));

           _NShape2->paint(GL_LINES);

        }
    }

    if(DicomRenderManager::getInstance().getDisplayFrameDetectionBox()){
        if(_cubeLeftN != nullptr){

            _lineShader->Set("projectionMatrix",_projection);
            _lineShader->Set("viewMatrix",_view);
            _lineShader->Set("worldMatrix",_data->getCTWorld());
            _lineShader->Set("color",Vec3f(1.0f,0,1.0f));

            _cubeLeftN->paint(GL_LINES);
        }

        if(_cubeRightN != nullptr){

            _lineShader->Set("projectionMatrix",_projection);
            _lineShader->Set("viewMatrix",_view);
            _lineShader->Set("worldMatrix",_data->getCTWorld());
            _lineShader->Set("color",Vec3f(1.0f,0,1.0f));

            _cubeRightN->paint(GL_LINES);
        }

    }

    _lineShader->Disable();

    _targetBinder->Unbind();
}


void DICOMRenderer::drawElectrodeCylinder(std::shared_ptr<GLFBOTex> target){
    _targetBinder->Bind(target);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    //draw the electrode basics

    _electrodeGeometryShader->Enable();
    _electrodeGeometryShader->Set("projectionMatrix",_projection);
    _electrodeGeometryShader->Set("viewMatrix",_view);

    //new mode!
    std::shared_ptr<BrainVisIO::MERData::MERBundle> bundle = BrainVisIO::MERData::MERBundleManager::getInstance().getMERBundle(BrainVisIO::MERData::MERBundleManager::getInstance().getActiveBundleName());
    if(bundle == nullptr) return;

    Vec3f t = bundle->getTarget();
    t -= Vec3f(100,100,100);
    t =  t.x*_data->getCTeX()+
         t.y*_data->getCTeY()+
         t.z*_data->getCTeZ()+
         (_data->getCTCenter()*_data->getCTScale());
    Vec3f e = bundle->getEntry();
    e -= Vec3f(100,100,100);
    e =  e.x*_data->getCTeX()+
         e.y*_data->getCTeY()+
         e.z*_data->getCTeZ()+
         (_data->getCTCenter()*_data->getCTScale());

    bundle->calculateElectrodePosition(_data->getCTeX(),_data->getCTeY(),t,e);

    Vec3f dir = e-t;
    float length = dir.length()+5;
    dir.normalize();

    Core::Math::Mat4f rotationmatrix = calculateElectrodeMatices(e,t);

    Mat4f translation;
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> currentElectrode;
    std::shared_ptr<BrainVisIO::MERData::MERData>      currentData;

    for(int i = 0; i < 3;++i){
        switch(i){
            case 0: currentElectrode = bundle->getElectrode("lat");break;
            case 1: currentElectrode = bundle->getElectrode("ant");break;
            case 2: currentElectrode = bundle->getElectrode("cen");break;
        }
        if(currentElectrode != nullptr && currentElectrode->getElectrodeVisible()){
            currentData = currentElectrode->getMERData(-10);
            if(currentData != nullptr){
                Vec3f p = currentData->getPosition();

                translation.Translation(p+ dir*length);

                _electrodeGeometryShader->Set("worldMatrix",rotationmatrix*translation);
                _electrodeGeometry->paint();
            }
        }
    }
    _electrodeGeometryShader->Disable();

    _targetBinder->Unbind();
}


void DICOMRenderer::drawElectrodeSpheres(std::shared_ptr<GLFBOTex> target){
    //testing elctronde mer manager ------------------------------------
    _targetBinder->Bind(target);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    Vec3f position;
    Mat4f scaleT;
    Mat4f transT;
    scaleT.Scaling(1.0,1.0,1.0);
    Mat4f worldScaling;
    worldScaling.Scaling(_data->getCTScale());
    Vec4f centerWorld = (worldScaling*Vec4f(_data->getCTCenter(),1));

    _sphereFFTShader->Enable();
    _sphereFFTShader->Set("projectionMatrix",_projection);
    _sphereFFTShader->Set("viewMatrix",_view);
    _sphereFFTShader->Set("fftRange",_data->getSpectralRange());
    _sphereFFTShader->SetTexture1D("fftColor",_FFTColor->GetGLID(),0);


    std::shared_ptr<BrainVisIO::MERData::MERBundle> bundle = BrainVisIO::MERData::MERBundleManager::getInstance().getMERBundle(BrainVisIO::MERData::MERBundleManager::getInstance().getActiveBundleName());
    if(bundle == nullptr) return;

    Vec3f t = bundle->getTarget();
    t -= Vec3f(100,100,100);
    t =  t.x*_data->getCTeX()+
         t.y*_data->getCTeY()+
         t.z*_data->getCTeZ()+
         (_data->getCTCenter()*_data->getCTScale());
    Vec3f e = bundle->getEntry();
    e -= Vec3f(100,100,100);
    e =  e.x*_data->getCTeX()+
         e.y*_data->getCTeY()+
         e.z*_data->getCTeZ()+
         (_data->getCTCenter()*_data->getCTScale());
    bundle->calculateElectrodePosition(_data->getCTeX(),_data->getCTeY(),t,e);

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat;
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant;
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen;

    lat = bundle->getElectrode("lat");
    ant = bundle->getElectrode("ant");
    cen = bundle->getElectrode("cen");

    for(int i = -10; i <= 5;++i){
		if (cen->getMERData(i) == nullptr) continue;

        if(cen->getElectrodeVisible()){
            position = cen->getMERData(i)->getPosition();
            transT.Translation(position);
            transT = scaleT*transT;
            _sphereFFTShader->Set("fftValue",(float)cen->getMERData(i)->getSpectralAverageNormalized());
            _sphereFFTShader->Set("worldMatrix",transT);
            _sphereFFTShader->Set("fftRange",Vec2f(0.0f,1.0f));

            _sphere->paint();
        }

        if(lat->getElectrodeVisible()){
            position = lat->getMERData(i)->getPosition();
            transT.Translation(position);
            transT = scaleT*transT;
            _sphereFFTShader->Set("fftValue",(float)lat->getMERData(i)->getSpectralAverageNormalized());
            _sphereFFTShader->Set("worldMatrix",transT);
            _sphereFFTShader->Set("fftRange",Vec2f(0.0f,1.0f));

            _sphere->paint();
        }

        if(ant->getElectrodeVisible()){
            position = ant->getMERData(i)->getPosition();
            transT.Translation(position);
            transT = scaleT*transT;
            _sphereFFTShader->Set("fftValue",(float)ant->getMERData(i)->getSpectralAverageNormalized());
            _sphereFFTShader->Set("worldMatrix",transT);
            _sphereFFTShader->Set("fftRange",Vec2f(0.0f,1.0f));

            _sphere->paint();
        }
    }




    _sphereFFTShader->Disable();

    _targetBinder->Unbind();
}


void DICOMRenderer::drawCompositing(){
    //------- Compositing
    _targetBinder->Unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, _displayFramebufferID);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    ClearBackground(Vec4f(0,1,1,0));
    _compositingThreeDShader->Enable();
    _compositingThreeDShader->Set("mrctblend",DicomRenderManager::getInstance().getBlendValue());
    _compositingThreeDShader->SetTexture2D("raycastCT",_rayCastColorCT->GetTextureHandle(),0);
    _compositingThreeDShader->SetTexture2D("raycastMR",_rayCastColorMR->GetTextureHandle(),1);
    _compositingThreeDShader->SetTexture2D("boundingBox",_boundingBoxVolumeBuffer->GetTextureHandle(),2);
    _compositingThreeDShader->SetTexture2D("fontTexture",_FontTexture->GetGLID(),3);
    _compositingThreeDShader->Set("blendoption",DicomRenderManager::getInstance().getBlendoption());

    _renderPlane->paint();

    _compositingThreeDShader->Disable();
}
//----------3D-Rendering-End--------------------------------------------------------------


void DICOMRenderer::SliceRendering(){

    if(_GL_CTVolume != nullptr){
        drawSlice(_sliceShader,true,true,false);

    }
    if(_GL_MRVolume != nullptr){   
        drawSlice(_sliceShader,false,false);
    }

    drawSliceCompositing();
}

void DICOMRenderer::drawSlice(std::shared_ptr<GLProgram> shader, bool isCT,bool full, bool noCTBones){
    Mat4f oldProjectionMatrix = _projection;
    Mat4f oldViewMatrix = _view;
    DICOMClipMode oldClipMode = _clipMode;

    Vec3f CTScaleOld = _data->getCTScale();
    Vec3f MRScaleOld = _data->getMRScale();


    float stepsize = 1;

    switch(_activeRenderMode){
    case RenderMode::XAxis :
        _projection.Ortho(-(int32_t)_windowSize.x/2*_vZoom.x,(int32_t)_windowSize.x/2*_vZoom.x,-(int32_t)_windowSize.y/2*_vZoom.x,(int32_t)_windowSize.y/2*_vZoom.x,-5000.0f,5000.0f);
        _view.BuildLookAt(_data->getSelectedWorldSpacePositon()+Vec3f(300,0,0),_data->getSelectedWorldSpacePositon(),Vec3f(0,0,1));

        //stepsize = _data->getCTScale().x*2;

        _clipMode = DICOMClipMode::PlaneX;
        break;

    case RenderMode::YAxis :
        _projection.Ortho(-(int32_t)_windowSize.x/2*_vZoom.y,(int32_t)_windowSize.x/2*_vZoom.y,-(int32_t)_windowSize.y/2*_vZoom.y,(int32_t)_windowSize.y/2*_vZoom.y,-5000.0f,5000.0f);
        _view.BuildLookAt(_data->getSelectedWorldSpacePositon()+Vec3f(0,300,0),_data->getSelectedWorldSpacePositon(),Vec3f(0,0,1));

        //stepsize = _data->getCTScale().y*2;

        _clipMode = DICOMClipMode::PlaneY;
        break;

    case RenderMode::ZAxis :
        _projection.Ortho(-(int32_t)_windowSize.x/2*_vZoom.z,(int32_t)_windowSize.x/2*_vZoom.z,-(int32_t)_windowSize.y/2*_vZoom.z,(int32_t)_windowSize.y/2*_vZoom.z,-5000.0f,5000.0f);
        _view.BuildLookAt(_data->getSelectedWorldSpacePositon()+Vec3f(0,0,-300),_data->getSelectedWorldSpacePositon(),Vec3f(0,-1,0));

        //stepsize = _data->getCTScale().z*2;

        _clipMode = DICOMClipMode::PlaneZ;
        break;

    }


    if(isCT){
        drawCubeEntry(_rayEntryCT,_data->getCTWorld());

        if(!noCTBones){
            isCT = false;
        }

        drawVolumeRayCast(  _rayCastColorCT,
                            _rayCastPositionCT,
                            shader,
                            _data->getCTWorld(),
                            _rayEntryCT->GetTextureHandle(),
                            _GL_CTVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getCTTransferScaling(),
                            isCT,
                            0.75);


    }else{
        drawCubeEntry(_rayEntryMR,_data->getMRWorld());

        drawVolumeRayCast(  _rayCastColorMR,
                            _rayCastPositionMR,
                            shader,
                            _data->getMRWorld(),
                            _rayEntryMR->GetTextureHandle(),
                            _GL_MRVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getMRTransferScaling(),
                            false,
                            0.75);
    }

    if(full){
        _targetBinder->Bind(_boundingBoxVolumeBuffer);
        ClearBackground(Vec4f(0,0,0,-800.0f));
        drawLineBoxes(_boundingBoxVolumeBuffer);
        drawElectrodeCylinder(_boundingBoxVolumeBuffer);
        drawElectrodeSpheres(_boundingBoxVolumeBuffer);

    }

    _projection = oldProjectionMatrix;
    _view = oldViewMatrix;
    _clipMode = oldClipMode;
}

void DICOMRenderer::drawSliceCompositing(){
  glBindFramebuffer(GL_FRAMEBUFFER, _displayFramebufferID);

  glCullFace(GL_BACK);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  ClearBackground(Vec4f(0,1,1,0));
  _compositingTwoDShader->Enable();
  _compositingTwoDShader->SetTexture2D("sliceImageCT",_rayCastColorCT->GetTextureHandle(),0);
  _compositingTwoDShader->SetTexture2D("sliceImageMR",_rayCastColorMR->GetTextureHandle(),1);
  _compositingTwoDShader->SetTexture2D("electrodeImage",_boundingBoxVolumeBuffer->GetTextureHandle(),2);
  _compositingTwoDShader->SetTexture2D("CTPosition",_rayCastPositionCT->GetTextureHandle(),3);
  _compositingTwoDShader->SetTexture2D("fontTexture",_FontTexture->GetGLID(),4);
  _compositingTwoDShader->Set("mrctblend", DicomRenderManager::getInstance().getBlendValue());

  _renderPlane->paint();

  _compositingTwoDShader->Disable();
}



void DICOMRenderer::updateTransferFunction(){
    std::vector<Core::Math::Vec4f> tf = *(DicomRenderManager::getInstance().getTransferFunction().get());

    if(tf.size() > 0){
        if(_transferFunctionTex == nullptr){
            _transferFunctionTex = std::make_shared<GLTexture1D>(tf.size(),GL_RGBA32F,GL_RGBA,GL_FLOAT,&(tf[0]));
        }else{
            _transferFunctionTex->SetData((void*)&(tf[0]));
        }
    }
}



void DICOMRenderer::PickPixel(Vec2ui coord){

    Vec4ui8 data(255,255,255,255);
    Vec3f VolumePos;

    _targetBinder->Unbind();

    _targetBinder->Bind(_rayCastPositionCT);
    glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);

    glReadPixels(coord.x, _windowSize.y - coord.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&data);

    if(data.x != 0 && data.y != 0 && data.z != 0){
        VolumePos.x = (float)data.x/255.0f;
        VolumePos.y = (float)data.y/255.0f;
        VolumePos.z = (float)data.z/255.0f;
    }


    _data->setSelectedSlices(VolumePos);


}

int sc = 0;
void DICOMRenderer::renderFramePosition(){
    Mat4f oldProjectionMatrix = _projection;
    Mat4f oldViewMatrix = _view;
    DICOMClipMode oldClipMode = _clipMode;
    RenderMode current = _activeRenderMode;

    _activeRenderMode = RenderMode::XAxis;
    int width = 250;
    int height = 250;

    _projection.Ortho(-(float)width/2*_vZoom.x,(float)width/2*_vZoom.x,-(float)height/2*_vZoom.x,(float)height/2*_vZoom.x,-5000.0f,5000.0f);
    _view.BuildLookAt(Vec3f(300,0,0),Vec3f(0,0,0),Vec3f(0,0,1));
    glViewport(0,0,width,height);

    float stepsize = 0;
    stepsize = _data->getCTScale().x*2;

    _clipMode = DICOMClipMode::PlaneX;

    drawCubeEntry(_rayEntryCT,_data->getCTWorld());

    drawVolumeRayCast(  _rayCastColorCT,
                        _rayCastPositionCT,
                        _frameSearchShader,
                        _data->getCTWorld(),
                        _rayEntryCT->GetTextureHandle(),
                        _GL_CTVolume->GetGLID(),
                        _transferFunctionTex->GetGLID(),
                        _data->getCTTransferScaling(),
                        true,
                        stepsize);


    glViewport(0,0,_windowSize.x,_windowSize.y);
    _projection = oldProjectionMatrix;
    _view = oldViewMatrix;
    _clipMode = oldClipMode;
    _activeRenderMode = current;
}


void DICOMRenderer::createFrameGeometry(std::vector<Vec3f> corners, int id){
    //create geometry
    float verts[3*2*3];
    int i = 0;
    verts[i++] =corners[0].x;
    verts[i++] =corners[0].y;
    verts[i++] =corners[0].z;

    verts[i++] =corners[3].x;
    verts[i++] =corners[3].y;
    verts[i++] =corners[3].z;

    verts[i++] =corners[3].x;
    verts[i++] =corners[3].y;
    verts[i++] =corners[3].z;

    verts[i++] =corners[1].x;
    verts[i++] =corners[1].y;
    verts[i++] =corners[1].z;

    verts[i++] =corners[1].x;
    verts[i++] =corners[1].y;
    verts[i++] =corners[1].z;

    verts[i++] =corners[2].x;
    verts[i++] =corners[2].y;
    verts[i++] =corners[2].z;


    int indices[6]{
        0,1,
        2,3,
        4,5
    };

   if(id == 0){
       _NShape1 = std::unique_ptr<GLModel>(new GLModel());
       _NShape1->Initialize(verts,indices,6,6);
   }else{
       _NShape2 = std::unique_ptr<GLModel>(new GLModel());
       _NShape2->Initialize(verts,indices,6,6);
   }
}


DICOMClipMode DICOMRenderer::clipMode() const
{
    return _clipMode;
}


void DICOMRenderer::setClipMode(const DICOMClipMode &clipMode)
{
    _clipMode = clipMode;
    _needsUpdate = true;
}


Core::Math::Mat4f DICOMRenderer::calculateElectrodeMatices(Core::Math::Vec3f entry, Core::Math::Vec3f target){
    //left
    Vec3f to,from;
    float elecLength,dot,cross;
    Mat4f Fm,G,U,S,T;

    to = target-entry;
    elecLength = to.length()+5;
    from = Vec3f(0,1,0);
    to.normalize();

    dot = from^to;
    cross = ((from%to).length());

    Fm;
    Fm.m11 = from.x;
    Fm.m21 = from.y;
    Fm.m31 = from.z;

    Fm.m12 = ((to - (from^to)*from) / ((to - (from^to)*from).length())).x;
    Fm.m22 = ((to - (from^to)*from) / ((to - (from^to)*from).length())).y;
    Fm.m32 = ((to - (from^to)*from) / ((to - (from^to)*from).length())).z;

    Fm.m13 = (to%from).x;
    Fm.m23 = (to%from).y;
    Fm.m33 = (to%from).z;


    G;
    G.m11 = dot;
    G.m12 = cross;
    G.m22 = dot;
    G.m21 = -cross;

    U = Fm*G*Fm.inverse();
    S.Scaling(Vec3f(1,elecLength,1));

    Vec3f outdir = ( entry-target);
    outdir.normalize();
    T.Translation(entry+  outdir*15.0f );

    return S*U; 
}

bool DICOMRenderer::doesGradientDescent() const
{
    return _doesGradientDescent;
}

void DICOMRenderer::setDoesGradientDescent(bool doesGradientDescent, FusionMode mode){
    _doesGradientDescent = doesGradientDescent;
    _fusionMode = mode;
}


void DICOMRenderer::generateLeftFrameBoundingBox(Vec3f center, Vec3f scale){
    Vec3f min(center.x-scale.x,center.y-scale.y,center.z-scale.z);
    Vec3f max(center.x+scale.x,center.y+scale.y,center.z+scale.z);
    if(_cubeLeftN == nullptr){
        _cubeLeftN = std::unique_ptr<GLBoundingBox>(new GLBoundingBox(min,max));
    }else{
        _cubeLeftN->update(min,max);
    }
}
void DICOMRenderer::generateRightFrameBoundingBox(Vec3f center, Vec3f scale){
    Vec3f min(center.x-scale.x,center.y-scale.y,center.z-scale.z);
    Vec3f max(center.x+scale.x,center.y+scale.y,center.z+scale.z);
    if(_cubeRightN == nullptr){
        _cubeRightN = std::unique_ptr<GLBoundingBox>(new GLBoundingBox(min,max));
    }else{
        _cubeRightN->update(min,max);
    }
}

std::vector<Vec3f> DICOMRenderer::findFrame(float startX, float stepX, Vec2f range, Vec3f minBox, Vec3f maxBox){
    //variable declaration
    bool foundStart = false;
    bool foundEnd = false;
    bool foundData = false;
    std::vector<Vec3f> frameData;
    std::vector<Vec4f> framebuffer;
    Vec3f currentSlide = Vec3f(startX,0.5,0.5);

    int width = 250;
    int height = 250;

    framebuffer.resize(width*height);


    _frameSearchShader->Enable();
    _frameSearchShader->Set("minBox",minBox);
    _frameSearchShader->Set("maxBox",maxBox);
    _frameSearchShader->Disable();

    while(!foundEnd && currentSlide.x >= 0.0f && currentSlide.x <= 1.0f){
        //render slide
        foundData = false;
        _data->setSelectedSlices(currentSlide);

        renderFramePosition();
        //render!!! somehow!!

        //read framebuffer
        _targetBinder->Bind(_rayCastColorCT);

        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, (GLvoid*)&(framebuffer[0]));

        for(int i = 0; i < framebuffer.size();++i){
            if(framebuffer[i].x == 0.0f && framebuffer[i].y == 0.0f && framebuffer[i].z == 0.0f){

            }else{
                frameData.push_back(Vec3f(framebuffer[i].x,framebuffer[i].y,framebuffer[i].z));
                foundData = true;
                foundStart = true;
            }
        }
        if(foundData || !foundStart){
             currentSlide.x += stepX*0.5f;
        }else{
            foundEnd = true;
        }

        _targetBinder->Unbind();
    }

    return frameData;
}

std::vector<Vec3f> DICOMRenderer::findFrameCorners(std::vector<Vec3f> data){
    //finding the corners of the N shape
    Vec3f topLeft(0,0,0);
    Vec3f bottomLeft(0,1,1);
    Vec3f topRight(0,0,0);
    Vec3f bottomRight(0,1,1);

    for(int i = 0; i < data.size();++i ){
        if(data[i].y < 0.5f && data[i].z > 0.5f ){
            if(data[i].z > topLeft.z)
                topLeft = data[i];
            //top left
        }else if(data[i].y > 0.5f && data[i].z > 0.5f ){
            if(data[i].z > topRight.z)
                topRight = data[i];
            //top right
        }else if(data[i].y < 0.5f && data[i].z < 0.5f ){
            if(data[i].z < bottomLeft.z)
                bottomLeft = data[i];

            //bottom left
        }else if(data[i].y > 0.5f && data[i].z < 0.5f ){
            if(data[i].z < bottomRight.z)
                bottomRight = data[i];
            //bottom right
        }
    }

    //transform from "volumespace" to "geometry cube space/worldspace"
    topLeft         -= 0.5f;
    topRight        -= 0.5f;
    bottomLeft      -= 0.5f;
    bottomRight     -= 0.5f;

    //push final frame position into vector vor later useage, allway clockwise
    // TopLeft->TopRight->BottomRight->BottomLeft
    std::vector<Vec3f> corners;
    corners.push_back(topLeft);
    corners.push_back(topRight);
    corners.push_back(bottomRight);
    corners.push_back(bottomLeft);

    return corners;
}

void DICOMRenderer::findGFrame(){
    if(!_doFrameDetection) return;
    Vec3f lMin = _data->getLeftFBBCenter()-_data->getLeftFBBScale();
    Vec3f lMax = _data->getLeftFBBCenter()+_data->getLeftFBBScale();

    Vec3f rMin = _data->getRightFBBCenter()-_data->getRightFBBScale();
    Vec3f rMax = _data->getRightFBBCenter()+_data->getRightFBBScale();

    std::vector<Vec3f> left = findFrame(lMin.x, 1.0f/_data->getCTScale().x, Vec2f(0.45f,0.6f), lMin, lMax);
    std::vector<Vec3f> right = findFrame(rMax.x, -1.0f/_data->getCTScale().x, Vec2f(0.45f,0.6f), rMin, rMax);

    std::vector<Vec3f> leftCorners = findFrameCorners(left);
    std::vector<Vec3f> rightCorners = findFrameCorners(right);


    _data->setLeftMarker(leftCorners);
    _data->setRightMarker(rightCorners);
    if(_data->calculateCTUnitVectors()){
        _data->setBFoundCTFrame(true);
    }

    // this has to be done if the frame was found
    if(_data->getBFoundCTFrame() && _GL_CTVolume != nullptr){
        createFrameGeometry(_data->getLeftMarker(),0);
        createFrameGeometry(_data->getRightMarker(),1);
    }

    _data->setSelectedSlices(Vec3f(0.5f,0.5f,0.5f));

    _doFrameDetection = false;
}

void DICOMRenderer::registerDatasets(){
        if(_doesGradientDescent){
            switch(_fusionMode){
                case FusionMode::OpenGL : _fusionMethod = std::unique_ptr<iFusion>(new OpenGLFusion(_data,
                                                                                               _windowSize,
                                                                                               _GL_CTVolume->GetGLID(),
                                                                                               _GL_MRVolume->GetGLID()));
                                            break;
                case FusionMode::CPU     :  _fusionMethod = std::unique_ptr<iFusion>(new CPUFusion(_data));break;
#ifdef SUPPORTS_CUDA
                case FusionMode::CUDA :   _fusionMethod = std::unique_ptr<iFusion>(new CudaFusion(_data));break;
#endif
                default :  _fusionMethod = std::unique_ptr<iFusion>(new OpenGLFusion(_data,_windowSize,_GL_CTVolume->GetGLID(),_GL_MRVolume->GetGLID()));break;
            }

            _fusionMethod->init(_windowSize);
            timer.start();
            _fusionMethod->execute();
            DicomRenderManager::getInstance().setBlendValue(0.5f);
            double d = timer.elapsed();
            std::cout << "end gdc : " << d<<std::endl;
            _doesGradientDescent = false;
        }
}

#include "DICOMRenderer.h"
#include <renderer/OpenGL/OpenGLDefines.h>
#include <algorithm>
#include <renderer/Context/ContextMutex.h>
#include <BrainVisIO/MER-Data/ElectrodeManager.h>

using namespace std;
using namespace Tuvok::Renderer;

DICOMRenderer::DICOMRenderer():
_activeRenderMode(ThreeDMode),
_windowSize(512,512),
_GL_CTVolume(nullptr),
_GL_MRVolume(nullptr),
_camera(new Camera(Vec3f(0,-400,0),Vec3f(0,0,0),Vec3f(0,0,1))),
_foundFrame(false),
_viewX(),
_viewY(),
_viewZ(),
_datasetStatus(0),
_clipMode(DICOMClipMode::none),
_electrodeGeometry(nullptr),
_vXZoom(2,2,2),
_vYZoom(2,2,2),
_vZZoom(2,2,2),
_needsUpdate(true)
{

}

void DICOMRenderer::Initialize(){
    //loading shaders, geometry and framebuffers
    if(!LoadShaderResources()) cout << "error in load shaders" << endl;
    if(!LoadGeometry()) cout << "error in load geometry" << endl;
    if(!LoadFrameBuffer()) cout << "error in load framebuffer" << endl;

    //creating a targetBinder to simplify switching between framebuffers
    _targetBinder = std::unique_ptr<GLTargetBinder>(new GLTargetBinder());


    //starting with some fix projection / view / viwport
    _projection.Perspective(45, 512.0f/512.0f,0.01f, 1000.0f);
    _view = _camera->buildLookAt();

    glViewport(0,0,512,512);
    sheduleRepaint();
}

void DICOMRenderer::Cleanup(){

}


void DICOMRenderer::SetRenderMode(RenderMode mode){
    _activeRenderMode = mode;
    sheduleRepaint();
}

void DICOMRenderer::SetWindowSize(uint32_t width, uint32_t height){
    //check if the size of the screen is changed
    if(width != _windowSize.x || height != _windowSize.y){
        _windowSize.x = width;
        _windowSize.y = height;

        //update projection and viewport
        _projection.Perspective(45, (float)width/(float)height,0.01f, 1000.0f);
        glViewport(0,0,width,height);

        //recreate the framebuffers
        LoadFrameBuffer();
    }
    sheduleRepaint();
}

void DICOMRenderer::SetDataHandle(std::shared_ptr<DataHandle> dataHandle){

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
    updateTransferFunction();

    // try to read the QT framebuffer id, it is != 0
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );

    // if this is the first renderer we have to search the G-Frame to
    // calculate the center of the scan
    if(!_data->getBFoundCTFrame() && _GL_CTVolume != nullptr){
       searchGFrame();

       // start slice is the center -> maybe change to center of frame!
       _data->setSelectedSlices(Vec3f(0.5f,0.5f,0.5f));
    }

    // this has to be done if the frame was found
    if(_data->getBFoundCTFrame() && _GL_CTVolume != nullptr){
        createFrameGeometry(_data->getLeftMarker(),0);
        createFrameGeometry(_data->getRightMarker(),1);
    }

    // creates a 1D Texture with a specified colorrange for
    // the filtered dB of the FFT
    if(!LoadFFTColorTex()) cout << "error in ffttex"<< endl;

    // sets the start rendermode
    _activeRenderMode = RenderMode::ThreeDMode;


    calculateElectrodeMatices();
    sheduleRepaint();
}

void DICOMRenderer::searchGFrame(Vec2f range){
    _data->setBFoundCTFrame(false);
    _data->setLeftMarker(findFrame(0.0f,1.0f,range));
    _data->setRightMarker(findFrame(1.0f,-1.0f,range));
    if(_data->calculateCTUnitVectors()){
        _data->setBFoundCTFrame(true);
    }
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
                                _vZZoom += Vec3f(z,z,z);
                                break;
        case RenderMode::YAxis :
                                _vYZoom += Vec3f(z,z,z);
                                 break;
        case RenderMode::XAxis :
                                _vXZoom += Vec3f(z,z,z);
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

void DICOMRenderer::Paint(){
    Tuvok::Renderer::Context::ContextMutex::getInstance().lockContext();
    _data->checkFocusPoint();

    checkDatasetStatus();

    //QT SUCKS and forces us to seperate into an else branch
    if(_needsUpdate){

        //check if the camera has to be placed automaticly
        if(ElectrodeManager::getInstance().isTrackingMode()){
            std::shared_ptr<iElectrode> elec = ElectrodeManager::getInstance().getElectrode(ElectrodeManager::getInstance().getTrackedElectrode());
            if(elec != nullptr){
                Vec3f lookAt = elec->getData(_data->getDisplayedDriveRange().y)->getDataPosition();

                lookAt = lookAt-Vec3f(100,100,100);
                lookAt =  _data->getCTeX()*lookAt.x +
                          _data->getCTeY()*lookAt.y +
                          _data->getCTeZ()*lookAt.z +
                          _data->getCTCenter()*_data->getCTScale();
                setCameraLookAt(lookAt);
            }
        }

        updateTransferFunction();

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
    vector<string> fs,vs;
    vs.push_back("Shader/CubeVertex.glsl");
    fs.push_back("Shader/CubeFragment.glsl");
    ShaderDescriptor sd(vs,fs);
    if(!LoadAndCheckShaders(_frontFaceShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/NearPlaneVertex.glsl");
    fs.push_back("Shader/NearPlaneFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_NearPlanShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/RayCasterVertex.glsl");
    fs.push_back("Shader/RayCasterFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_rayCastShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/ComposeVertex.glsl");
    fs.push_back("Shader/ComposeFragment_ThreeD.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_compositingThreeDShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/ComposeVertex.glsl");
    fs.push_back("Shader/ComposeFragment_TwoD.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_compositingTwoDShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/SliceVertex.glsl");
    fs.push_back("Shader/SliceFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_sliceShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/LineVertex.glsl");
    fs.push_back("Shader/LineFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_lineShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/FFTColorVertex.glsl");
    fs.push_back("Shader/FFTColorFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_sphereFFTShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/FrameFindVertex.glsl");
    fs.push_back("Shader/FrameFindFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_frameSearchShader,sd)) return false;

    vs.clear();
    fs.clear();
    vs.push_back("Shader/CubeVertex.glsl");
    fs.push_back("Shader/ElectrodeGeometryFragment.glsl");
    sd = ShaderDescriptor(vs,fs);
    if(!LoadAndCheckShaders(_electrodeGeometryShader,sd)) return false;

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
    _volumeBox = std::unique_ptr<GLVolumeBox>(new GLVolumeBox());
    _renderPlane = std::unique_ptr<GLRenderPlane>(new GLRenderPlane(_windowSize));
    _renderPlaneX = std::unique_ptr<GLRenderPlane>(new GLRenderPlane(_windowSize,0));
    _renderPlaneY = std::unique_ptr<GLRenderPlane>(new GLRenderPlane(_windowSize,1));
    _renderPlaneZ = std::unique_ptr<GLRenderPlane>(new GLRenderPlane(_windowSize,2));
    _boundingBox = std::unique_ptr<GLBoundingBox>(new GLBoundingBox());
    _XAxisSlice = std::unique_ptr<GLBoundingQuadX>(new GLBoundingQuadX());
    _YAxisSlice = std::unique_ptr<GLBoundingQuadY>(new GLBoundingQuadY());
    _ZAxisSlice = std::unique_ptr<GLBoundingQuadZ>(new GLBoundingQuadZ());
    _sphere = std::unique_ptr<GLSphere>(new GLSphere());

    _electrodeGeometry = std::unique_ptr<GLCylinder>(new GLCylinder(0.5f,20));

    return true;
}


bool DICOMRenderer::LoadFrameBuffer(){
     _rayEntryCT                    = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastColorCT                = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastPositionCT             = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayEntryMR                    = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastColorMR                = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _rayCastPositionMR             = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDCTVolumeFBO               = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDCTPositionVolumeFBO       = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDMRVolumeFBO               = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDMRPositionVolumeFBO       = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDElectrodeFBO              = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDFontFBO                   = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _TwoDTopFBO                    = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
     _boundingBoxVolumeBuffer       = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);

     //fontbuffer
     _FontTexture                   = std::make_shared<GLTexture2D>(_windowSize.x, _windowSize.y, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
     return true;
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

    if(_GL_CTVolume != nullptr && _data->getMRCTBlend() > 0.0f){
        drawCubeEntry(_rayEntryCT,_data->getCTWorld());

        drawVolumeRayCast(  _rayCastColorCT,
                            _rayCastPositionCT,
                            _data->getCTWorld(),
                            _rayEntryCT->GetTextureHandle(),
                            _GL_CTVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getCTTransferScaling());
    }

    if(_GL_MRVolume != nullptr && _data->getMRCTBlend() < 1.0f){
        drawCubeEntry(_rayEntryMR,_data->getMRWorld());

        drawVolumeRayCast(  _rayCastColorMR,
                            _rayCastPositionMR,
                            _data->getMRWorld(),
                            _rayEntryMR->GetTextureHandle(),
                            _GL_MRVolume->GetGLID(),
                            _transferFunctionTex->GetGLID(),
                            _data->getMRTransferScaling());
    }

    drawLineBoxes();

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
                                        Mat4f world,
                                        GLuint entryHandle,
                                        GLuint volumeHandle,
                                        GLuint tfHandle,
                                        float tfScaling){
    _targetBinder->Bind(colorTarget,positionTarget);
    ClearBackground(Vec4f(0,0,0,-1000.0f));


    glCullFace(GL_FRONT);
    _rayCastShader->Enable();
    _rayCastShader->Set("projectionMatrix",_projection);
    _rayCastShader->Set("viewMatrix",_view);
    _rayCastShader->Set("worldMatrix",world);
    _rayCastShader->SetTexture2D("rayStartPoint",entryHandle,0);
    _rayCastShader->SetTexture3D("volume",volumeHandle,1);
    _rayCastShader->SetTexture1D("transferfunction",tfHandle,2);
    _rayCastShader->Set("worldFragmentMatrix",world);
    _rayCastShader->Set("viewFragmentMatrix",_view);
    _rayCastShader->Set("tfScaling",tfScaling);
    _rayCastShader->Set("eyePos",_camera->GetWorldPosition());
    _rayCastShader->Set("focusWorldPos",_data->getSelectedWorldSpacePositon());

    switch(_clipMode){
    case DICOMClipMode::none :      _rayCastShader->Set("cutMode",0); break;
    case DICOMClipMode::CubicCut :  _rayCastShader->Set("cutMode",1); break;

    case DICOMClipMode::PlaneX :
                                    _rayCastShader->Set("cutPlaneNormal",_data->getCTeX());
                                    _rayCastShader->Set("cutMode",2);
                                    break;

    case DICOMClipMode::PlaneY :
                                    _rayCastShader->Set("cutPlaneNormal",_data->getCTeY());
                                    _rayCastShader->Set("cutMode",2);
                                    break;

    case DICOMClipMode::PlaneZ :
                                    _rayCastShader->Set("cutPlaneNormal",_data->getCTeZ());
                                    _rayCastShader->Set("cutMode",2);
                                    break;
    case DICOMClipMode::PlaneXn :
                                    _rayCastShader->Set("cutPlaneNormal",-_data->getCTeX());
                                    _rayCastShader->Set("cutMode",2);
                                    break;

    case DICOMClipMode::PlaneYn :
                                    _rayCastShader->Set("cutPlaneNormal",-_data->getCTeY());
                                    _rayCastShader->Set("cutMode",2);
                                    break;

    case DICOMClipMode::PlaneZn :
                                    _rayCastShader->Set("cutPlaneNormal",-_data->getCTeZ());
                                    _rayCastShader->Set("cutMode",2);
                                    break;
    default:                        _rayCastShader->Set("cutMode",0); break;
    }

    _volumeBox->paint();

    _rayCastShader->Disable();

    _targetBinder->Unbind();
}



void DICOMRenderer::drawLineBoxes(){
    //------- Bounding Box for CT Volume
    _targetBinder->Bind(_boundingBoxVolumeBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    ClearBackground(Vec4f(0,0,0,-800.0f));
    _lineShader->Enable();

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
    _lineShader->Set("color",Vec3f(1,0,0));
    _XAxisSlice->paint(GL_LINES);

    translation.Translation(0,currentSlices.y-0.5f,0);
    sliceWorld = translation*_data->getCTWorld();
    _lineShader->Set("worldMatrix",sliceWorld);
    _lineShader->Set("color",Vec3f(0,1,0));
    _YAxisSlice->paint(GL_LINES);

    translation.Translation(0,0,currentSlices.z-0.5f);
    sliceWorld = translation*_data->getCTWorld();
    _lineShader->Set("worldMatrix",sliceWorld);
    _lineShader->Set("color",Vec3f(0,0,1));
    _ZAxisSlice->paint(GL_LINES);

    _lineShader->Disable();
    _targetBinder->Unbind();
}


void DICOMRenderer::drawPlaning(){
    //we draw the electrode/g frame in the same FBO as the bounding box
    //could be changed later
    _targetBinder->Bind(_boundingBoxVolumeBuffer);
    drawGFrame();

    drawCenterCube();

    drawElectrodeCylinder();

    drawElectrodeSpheres();

    _targetBinder->Unbind();
}


void DICOMRenderer::drawCenterCube(){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    //draws a cube in the center (coord 100,100,100 in gframe)
    Mat4f scaleT;
    scaleT.Scaling(1.0,1.0,1.0);
    Mat4f transT;
    transT.Translation(_data->getLeftSTN()._endWorldSpace);
    transT = scaleT*transT;

    _frontFaceShader->Enable();
    _frontFaceShader->Set("projectionMatrix",_projection);
    _frontFaceShader->Set("viewMatrix",_view);
    transT.Translation(_data->getCTCenter()*_data->getCTScale());
    transT = scaleT*transT;

    _frontFaceShader->Set("worldMatrix",transT);
    _volumeBox->paint();

    _frontFaceShader->Disable();
}


void DICOMRenderer::drawGFrame(){
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    //check if we found the G Frame and draws it if found-------------
    _lineShader->Enable();
    if(_NShape1 != nullptr){
        _lineShader->Enable();

        _lineShader->Set("projectionMatrix",_projection);
        _lineShader->Set("viewMatrix",_view);
        _lineShader->Set("worldMatrix",_data->getCTWorld());
        _lineShader->Set("color",Vec3f(0.5f,1,0.5f));

        _NShape1->paint(GL_LINES);

    }

    if(_NShape2 != nullptr){
        _lineShader->Enable();

        _lineShader->Set("projectionMatrix",_projection);
        _lineShader->Set("viewMatrix",_view);
        _lineShader->Set("worldMatrix",_data->getCTWorld());
        _lineShader->Set("color",Vec3f(0.5f,1,0.5f));

       _NShape2->paint(GL_LINES);

    }
    _lineShader->Disable();
}


void DICOMRenderer::drawElectrodeCylinder(){
    //draw the electrode basics

    _electrodeGeometryShader->Enable();
    _electrodeGeometryShader->Set("projectionMatrix",_projection);
    _electrodeGeometryShader->Set("viewMatrix",_view);

    Vec3f dirLeft = _data->getLeftSTN()._startWorldSpace-_data->getLeftSTN()._endWorldSpace;
    float lengthLeft = dirLeft.length()+5;
    dirLeft.normalize();

    Vec3f dirRight = _data->getRightSTN()._startWorldSpace-_data->getRightSTN()._endWorldSpace;
    float lengthRight = dirRight.length()+5;
    dirRight.normalize();

    Vec3f startPos;
    Mat4f translation;

    for(int i = 0; i < ElectrodeManager::getInstance().getElectrodeCount();++i){
        std::shared_ptr<iElectrode> electrode = _data->getElectrode(i);
        if(!electrode->getIsSelected()) continue;
        if((electrode->getName().c_str())[0] == 'L'){
            startPos = electrode->getData(electrode->getDepthRange().x)->getDataPosition();
            startPos = (startPos-Vec3f(100,100,100));
            startPos =  startPos.x*_data->getCTeX()+
                        startPos.y*_data->getCTeY()+
                        startPos.z*_data->getCTeZ()+
                        (_data->getCTCenter()*_data->getCTScale());
            translation.Translation(startPos+dirLeft*lengthLeft);

            _electrodeGeometryShader->Set("worldMatrix",_electrodeLeftMatrix*translation);
            _electrodeGeometry->paint();
        }else if((electrode->getName().c_str())[0] == 'R'){
            startPos = electrode->getData(electrode->getDepthRange().x)->getDataPosition();
            startPos = (startPos-Vec3f(100,100,100));
            startPos =  startPos.x*_data->getCTeX()+
                        startPos.y*_data->getCTeY()+
                        startPos.z*_data->getCTeZ()+
                        (_data->getCTCenter()*_data->getCTScale());
            translation.Translation(startPos+dirRight*lengthRight);

            _electrodeGeometryShader->Set("worldMatrix",_electrodeRightMatix*translation);
            _electrodeGeometry->paint();
        }
    }
    _electrodeGeometryShader->Disable();
}


void DICOMRenderer::drawElectrodeSpheres(){
    //draw the electrodes----------------------------
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

    for(int i = 0; i < ElectrodeManager::getInstance().getElectrodeCount();i++){
        std::shared_ptr<iElectrode> electrode = _data->getElectrode(i);
        if(!electrode->getIsSelected()) continue;
        if(electrode != nullptr){
            for(int k = (int)electrode->getDepthRange().x; k <= _data->getDisplayedDriveRange().y; ++k){
                std::shared_ptr<iMERData> data = electrode->getData(k);
                if(data != nullptr){

                    position = data->getDataPosition()-Vec3f(100,100,100);
                    position = centerWorld.xyz() + _data->getCTeX()*position.x + _data->getCTeY()*position.y + _data->getCTeZ()*position.z;
                    transT.Translation(position);
                    transT = scaleT*transT;
                    _sphereFFTShader->Set("fftValue",(float)data->getSpectralAverage());
                    _sphereFFTShader->Set("worldMatrix",transT);
                    _sphereFFTShader->Set("fftRange",Vec2f(electrode->getSpectralPowerRange().x,electrode->getSpectralPowerRange().y));

                    _sphere->paint();
                }
            }
        }
    }

    _sphereFFTShader->Disable();
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
    _compositingThreeDShader->Set("mrctblend",_data->getMRCTBlend());
    _compositingThreeDShader->SetTexture2D("raycastCT",_rayCastColorCT->GetTextureHandle(),0);
    _compositingThreeDShader->SetTexture2D("raycastMR",_rayCastColorMR->GetTextureHandle(),1);
    _compositingThreeDShader->SetTexture2D("boundingBox",_boundingBoxVolumeBuffer->GetTextureHandle(),2);
    _compositingThreeDShader->SetTexture2D("fontTexture",_FontTexture->GetGLID(),3);

    _renderPlane->paint();

    _compositingThreeDShader->Disable();
}
//----------3D-Rendering-End--------------------------------------------------------------


void DICOMRenderer::SliceRendering(){

    if(_GL_CTVolume != nullptr){
            drawSlice(                      _GL_CTVolume->GetGLID(),
                                            _data->getCTTransferScaling(),
                                            _TwoDCTVolumeFBO,
                                            _TwoDCTPositionVolumeFBO,
                                            Vec3f(0,0,0),
                                            _data->getCTScale());

    }
    if(_GL_MRVolume != nullptr){
            drawSlice(                  _GL_MRVolume->GetGLID(),
                                        _data->getMRTransferScaling(),
                                        _TwoDMRVolumeFBO,
                                        _TwoDMRPositionVolumeFBO,
                                        _data->getMROffset(),
                                        _data->getMRScale(),
                                        true);
    }
    drawSliceElectrode();

    drawSliceCompositing();
}


void DICOMRenderer::drawSlice(GLuint volumeID,
                                float transferScaling,
                                std::shared_ptr<GLFBOTex> color,
                                std::shared_ptr<GLFBOTex> position,
                                Vec3f VolumeTranslation,
                                Vec3f VolumeScale,
                                bool secondary){
    _targetBinder->Bind(color,position);
    ClearBackground(Vec4f(0,0,0,-2000));

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    Mat4f scale;
    Mat4f zoom;
    Mat4f offsetTranslation;
    Mat4f translation;
    Mat4f rotation;
    scale.Scaling(VolumeScale);

    offsetTranslation.Translation(VolumeTranslation);


    Vec3f slice = _data->getSelectedSlices();
    if(secondary){
        slice-= Vec3f(0.5f,0.5f,0.5f);
        slice*= _data->getCTScale(); //<- worldposition of non rotated ct volume!

        slice /= _data->getMRScale();
        slice += Vec3f(0.5,0.5,0.5); //<- mr volume pos;


        VolumeTranslation/=VolumeScale;

        Mat4f x,y,z;
        x.RotationX(_data->getMRRotation().x);
        y.RotationY(_data->getMRRotation().y);
        z.RotationZ(_data->getMRRotation().z);
        rotation = x*y*z;
    }

    if(_GL_CTVolume != nullptr){
        _sliceShader->Enable();
        _sliceShader->SetTexture3D("volume",volumeID,0);
        _sliceShader->SetTexture1D("transferfunction",_transferFunctionTex->GetGLID(),1);
        _sliceShader->Set("tfScaling",transferScaling);

        _sliceShader->Set("target1",_data->getLeftSTN()._endVolumeSpace);
        _sliceShader->Set("target2",_data->getRightSTN()._endVolumeSpace);
        _sliceShader->Set("entry1",_data->getLeftSTN()._startVolumeSpace);
        _sliceShader->Set("entry2",_data->getRightSTN()._startVolumeSpace);
        _sliceShader->Set("radius",0.03f);

        Vec3f CenterOffset = (_data->getSelectedSlices()-Vec3f(0.5f,0.5f,0.5f))*_data->getCTScale();

        switch(_activeRenderMode){
            case RenderMode::ZAxis :{
                                    //z dependend
                                    _orthographicZAxis.Ortho(-(int32_t)_windowSize.x/2,(int32_t)_windowSize.x/2,-(int32_t)_windowSize.y/2,(int32_t)_windowSize.y/2,-5000.0f,5000.0f);
                                    _viewZ.BuildLookAt((Vec3f(0,0,100)*zoom)+CenterOffset,Vec3f(0,0,0)+CenterOffset,Vec3f(0,1,0));
                                    translation.Translation(VolumeTranslation+Vec3f(0,0,slice.z-0.5f));

                                    zoom.Scaling(_vZZoom);

                                    _sliceShader->Set("axis",2);
                                    _sliceShader->Set("projection",_orthographicZAxis);

                                    _sliceShader->Set("view",_viewZ*zoom);

                                    _sliceShader->Set("rotation",rotation);
                                    _sliceShader->Set("scale",scale);
                                    _sliceShader->Set("translate",translation);
                                    _sliceShader->Set("volumeOffset",VolumeTranslation);
                                    _sliceShader->Set("slide",slice.z-VolumeTranslation.z);
                                    _renderPlaneZ->paint();

                                    break;
                                    }
            case RenderMode::XAxis :{
                                    //x dependend
                                    _orthographicXAxis.Ortho(-(int32_t)_windowSize.x/2,(int32_t)_windowSize.x/2,-(int32_t)_windowSize.y/2,(int32_t)_windowSize.y/2,-5000.0f,5000.0f);
                                    _viewX.BuildLookAt((Vec3f(100,0,0)*zoom)+CenterOffset,Vec3f(0,0,0)+CenterOffset,Vec3f(0,0,1));
                                    translation.Translation(VolumeTranslation+Vec3f(slice.x-0.5f,0,0));

                                    zoom.Scaling(_vXZoom);

                                    _sliceShader->Set("axis",0);
                                    _sliceShader->Set("projection",_orthographicXAxis);
                                    _sliceShader->Set("view",_viewX*zoom);

                                    _sliceShader->Set("rotation",rotation);
                                    _sliceShader->Set("scale",scale);
                                    _sliceShader->Set("translate",translation);
                                    _sliceShader->Set("volumeOffset",VolumeTranslation);
                                    _sliceShader->Set("slide",slice.x-VolumeTranslation.x);
                                    _renderPlaneX->paint();

                                    break;
                                    }
            case RenderMode::YAxis :{
                                    //y dependend
                                    _orthographicYAxis.Ortho(-(int32_t)_windowSize.x/2,(int32_t)_windowSize.x/2,-(int32_t)_windowSize.y/2,(int32_t)_windowSize.y/2,-5000.0f,5000.0f);
                                    _viewY.BuildLookAt((Vec3f(0,-100,0)*zoom)+CenterOffset,Vec3f(0,0,0)+CenterOffset,Vec3f(0,0,1));
                                    translation.Translation(VolumeTranslation+Vec3f(0,slice.y-0.5f,0));

                                    zoom.Scaling(_vYZoom);

                                    _sliceShader->Set("axis",1);
                                    _sliceShader->Set("projection",_orthographicYAxis);
                                    _sliceShader->Set("view",_viewY*zoom);

                                    _sliceShader->Set("rotation",rotation);
                                    _sliceShader->Set("scale",scale);
                                    _sliceShader->Set("translate",translation);
                                    _sliceShader->Set("volumeOffset",VolumeTranslation);
                                    _sliceShader->Set("slide",slice.y-VolumeTranslation.y);
                                    _renderPlaneY->paint();

                                    break;
                                    }
        };
    }

    glDisable(GL_DEPTH_TEST);
}


void DICOMRenderer::drawSliceElectrode(){

    _targetBinder->Bind(_TwoDElectrodeFBO);
    ClearBackground(Vec4f(0,0,0,-90000));

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    Mat4f zoom;


    Mat4f curProj;
    Mat4f curView;

    switch(_activeRenderMode){
        case RenderMode::ZAxis :{
                                curProj = _orthographicZAxis;
                                zoom.Scaling(_vZZoom);
                                curView = _viewZ*zoom;
                                break;
                                }
        case RenderMode::XAxis :{
                                curProj = _orthographicXAxis;
                                zoom.Scaling(_vXZoom);
                                curView = _viewX*zoom;
                                break;
                                }
        case RenderMode::YAxis :{
                                curProj = _orthographicYAxis;
                                zoom.Scaling(_vYZoom);
                                curView = _viewY*zoom;
                                break;
                                }
    };

    _electrodeGeometryShader->Enable();
    _electrodeGeometryShader->Set("projectionMatrix",curProj);
    _electrodeGeometryShader->Set("viewMatrix",curView);

    Vec3f dirLeft = _data->getLeftSTN()._startWorldSpace-_data->getLeftSTN()._endWorldSpace;
    float lengthLeft = dirLeft.length()+5;
    dirLeft.normalize();

    Vec3f dirRight = _data->getRightSTN()._startWorldSpace-_data->getRightSTN()._endWorldSpace;
    float lengthRight = dirRight.length()+5;
    dirRight.normalize();

    Vec3f startPos;
    Mat4f translation;

    for(int i = 0; i < ElectrodeManager::getInstance().getElectrodeCount();++i){
        std::shared_ptr<iElectrode> electrode = _data->getElectrode(i);
        if(!electrode->getIsSelected()) continue;
        if((electrode->getName().c_str())[0] == 'L'){
            startPos = electrode->getData(electrode->getDepthRange().x)->getDataPosition();
            startPos = (startPos-Vec3f(100,100,100));
            startPos =  startPos.x*_data->getCTeX()+
                        startPos.y*_data->getCTeY()+
                        startPos.z*_data->getCTeZ()+
                        (_data->getCTCenter()*_data->getCTScale());
            translation.Translation(startPos+dirLeft*lengthLeft);

            _electrodeGeometryShader->Set("worldMatrix",_electrodeLeftMatrix*translation);
            _electrodeGeometry->paint();
        }else if((electrode->getName().c_str())[0] == 'R'){
            startPos = electrode->getData(electrode->getDepthRange().x)->getDataPosition();
            startPos = (startPos-Vec3f(100,100,100));
            startPos =  startPos.x*_data->getCTeX()+
                        startPos.y*_data->getCTeY()+
                        startPos.z*_data->getCTeZ()+
                        (_data->getCTCenter()*_data->getCTScale());
            translation.Translation(startPos+dirRight*lengthRight);

            _electrodeGeometryShader->Set("worldMatrix",_electrodeRightMatix*translation);
            _electrodeGeometry->paint();
        }
    }
    _electrodeGeometryShader->Disable();


    //draw the trajectories
    Vec3f position;
    Mat4f scaleT;
    Mat4f transT;
    scaleT.Scaling(1.0,1.0,1.0);
    Mat4f worldScaling;
    worldScaling.Scaling(_data->getCTScale());
    Vec4f centerWorld = (worldScaling*Vec4f(_data->getCTCenter(),1));

    _sphereFFTShader->Enable();
    _sphereFFTShader->Set("projectionMatrix",curProj);
    _sphereFFTShader->Set("viewMatrix",curView);
    _sphereFFTShader->Set("fftRange",_data->getSpectralRange());
    _sphereFFTShader->SetTexture1D("fftColor",_FFTColor->GetGLID(),0);

    for(int i = 0; i < 6;i++){
        std::shared_ptr<iElectrode> electrode = _data->getElectrode(i);
        if(!electrode->getIsSelected()) continue;
        if(electrode != nullptr){
            for(int k = (int)electrode->getDepthRange().x; k <= _data->getDisplayedDriveRange().y; ++k){
                std::shared_ptr<iMERData> data = electrode->getData(k);
                if(data != nullptr){

                    position = data->getDataPosition()-Vec3f(100,100,100);
                    position = centerWorld.xyz() + _data->getCTeX()*position.x + _data->getCTeY()*position.y + _data->getCTeZ()*position.z;
                    transT.Translation(position);
                    transT = scaleT*transT;
                    _sphereFFTShader->Set("fftValue",(float)data->getSpectralAverage());
                    _sphereFFTShader->Set("worldMatrix",transT);
                    _sphereFFTShader->Set("fftRange",Vec2f(electrode->getSpectralPowerRange().x,electrode->getSpectralPowerRange().y));

                    _sphere->paint();
                }
            }
        }
    }

    _sphereFFTShader->Disable();


    glDisable(GL_DEPTH_TEST);
}


void DICOMRenderer::drawSliceTop(){

}


void DICOMRenderer::drawSliceFont(){

}


void DICOMRenderer::drawSliceCompositing(){
  glBindFramebuffer(GL_FRAMEBUFFER, _displayFramebufferID);

  glCullFace(GL_BACK);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  ClearBackground(Vec4f(0,1,1,0));
  _compositingTwoDShader->Enable();
  _compositingTwoDShader->SetTexture2D("sliceImageCT",_TwoDCTVolumeFBO->GetTextureHandle(),0);
  _compositingTwoDShader->SetTexture2D("sliceImageMR",_TwoDMRVolumeFBO->GetTextureHandle(),1);
  _compositingTwoDShader->SetTexture2D("electrodeImage",_TwoDElectrodeFBO->GetTextureHandle(),2);
  _compositingTwoDShader->SetTexture2D("CTPosition",_TwoDCTPositionVolumeFBO->GetTextureHandle(),3);
  _compositingTwoDShader->SetTexture2D("topImage",_TwoDTopFBO->GetTextureHandle(),4);
  _compositingTwoDShader->Set("mrctblend", _data->getMRCTBlend());

  Vec3f slice = _data->getSelectedSlices();
  slice-= Vec3f(0.5f,0.5,0.5f);
  float zoom = 0;

  switch(_activeRenderMode){
    case RenderMode::ZAxis :slice.z = 100000.0f; zoom = _vZZoom.z; break;
    case RenderMode::XAxis :slice.x = 100000.0f; zoom = _vXZoom.x;break;
    case RenderMode::YAxis :slice.y = 100000.0f; zoom = _vYZoom.y;break;
  }
  _compositingTwoDShader->Set("focusPoint",slice);
  _compositingTwoDShader->Set("zoomFactor",zoom);



  _renderPlane->paint();

  _compositingTwoDShader->Disable();
}


void DICOMRenderer::frameSlicing(Vec2f range){
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );

    ClearBackground(Vec4f(0,0,0,0));

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    if(_GL_CTVolume != nullptr){
        Mat4f scale;
        Mat4f translation;
        Mat4f world;

        float longest = std::max(_data->getCTScale().x,std::max(_data->getCTScale().y,_data->getCTScale().z));

        translation.Translation(_data->getCTOffset().y,_data->getCTOffset().z,0);
        scale.Scaling(Vec3f(_data->getCTScale().y/longest,_data->getCTScale().z/longest,1));
        world = scale*translation;

        _frameSearchShader->Enable();

        _frameSearchShader->SetTexture3D("volume",_GL_CTVolume->GetGLID(),0);
        _frameSearchShader->Set("tfScaling",_data->getCTTransferScaling());
        _frameSearchShader->Set("world",world);
        _frameSearchShader->Set("projection",_orthographicXAxis);
        _frameSearchShader->Set("slide",_data->getSelectedSlices().x);
        _frameSearchShader->Set("frameRange",range);

        _renderPlane->paint();

        _frameSearchShader->Disable();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _displayFramebufferID);
}


void DICOMRenderer::updateTransferFunction(){
    std::vector<Core::Math::Vec4f> tf = *(_data->getTransferFunction().get());
    if(tf.size() > 0){
        if(_transferFunctionTex == nullptr){
            _transferFunctionTex = std::make_shared<GLTexture1D>(tf.size(),GL_RGBA32F,GL_RGBA,GL_FLOAT,&(tf[0]));
        }else{
            _transferFunctionTex->SetData((void*)&(tf[0]));
        }
    }
}



void DICOMRenderer::PickPixel(Vec2ui coord){
    Tuvok::Renderer::Context::ContextMutex::getInstance().lockContext();
    Vec4ui8 data(255,255,255,255);
    Vec4f VolumePos;

    if(_activeRenderMode == RenderMode::ThreeDMode){
        _targetBinder->Bind(_rayCastPositionCT);
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(coord.x, _windowSize.y - coord.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&data);

        if(data.x != 0 && data.y != 0 && data.z != 0)
            VolumePos = Vec4f((float)data.x/255.0f,(float)data.y/255.0f,(float)data.z/255.0f,(float)data.w/255.0f);



        _data->setSelectedSlices(Vec3f(VolumePos.x,VolumePos.y,VolumePos.z));

        _targetBinder->Unbind();
    }else{
        _targetBinder->Bind(_TwoDCTPositionVolumeFBO);
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(coord.x, _windowSize.y - coord.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&data);

        if(data.x != 0 && data.y != 0 && data.z != 0){
            VolumePos = Vec4f((float)data.x/255.0f,(float)data.y/255.0f,(float)data.z/255.0f,(float)data.w/255.0f);
            std::cout << VolumePos << "    -    "   << _data->getSelectedSlices() << std::endl;
            if(_activeRenderMode == RenderMode::XAxis && VolumePos.x == _data->getSelectedSlices().x){
                _data->setSelectedSlices(VolumePos.xyz());
            }else if(_activeRenderMode == RenderMode::YAxis && VolumePos.y == _data->getSelectedSlices().y){
                _data->setSelectedSlices(VolumePos.xyz());
            }else if(_activeRenderMode == RenderMode::ZAxis && VolumePos.z == _data->getSelectedSlices().z){
                _data->setSelectedSlices(VolumePos.xyz());
            }
        }
        _targetBinder->Unbind();
    }
Tuvok::Renderer::Context::ContextMutex::getInstance().unlockContext();
}


std::vector<Vec3f> DICOMRenderer::findFrame(float startX, float stepX, Vec2f range){
    bool firstFindEnd = false;
    bool firstFindStart = false;
    bool foundOnSlice = false;
    std::vector<Vec4ui8> framebuffer;
    std::vector<Vec3f> framePosition;
    Vec3f curValue;
    Vec3f currentSlices(startX,0,0);

    _data->setSelectedSlices(currentSlices);
    //_activeRenderMode = RenderMode::XAxis;
    while(!firstFindEnd){
        //render x slice
        //SliceRendering(1);
        frameSlicing(range);

        //read framebuffer
        framebuffer.clear();
        framebuffer.resize(_windowSize.x*_windowSize.y);
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &_displayFramebufferID );
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, _windowSize.x, _windowSize.y, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&(framebuffer[0]));

        foundOnSlice = false;
        for(int i = 0; i < framebuffer.size();++i){
            curValue = Vec3f((float)framebuffer[i].x/255.0f,(float)framebuffer[i].y/255.0f,(float)framebuffer[i].z/255.0f);
            if(curValue.x == 0.0f && curValue.y == 1.0f && curValue.z == 1.0f){
                //not frame
            }else if(curValue.x == 0.0f && curValue.y == 0.0f && curValue.z == 0.0f){
                //not part of volume
            }else {
                foundOnSlice = true;
                framePosition.push_back(curValue);
            }
        }


        //go to next slice
        currentSlices = _data->getSelectedSlices();
        currentSlices.x += stepX * (1.0f/(float)_data->getCTDimensions().x);
        _data->setSelectedSlices(currentSlices);

        if(foundOnSlice && !firstFindStart){
            firstFindStart = true;
        }
        if(!foundOnSlice && firstFindStart){
            firstFindEnd = true;
        }
    }

    //finding the corners of the N shape
    Vec3f topLeft(0,0,0);
    Vec3f bottomLeft(0,1,1);
    Vec3f topRight(0,0,0);
    Vec3f bottomRight(0,1,1);

    for(int i = 0; i < framePosition.size();++i ){
        if(framePosition[i].y < 0.5f && framePosition[i].z > 0.5f ){
            if(framePosition[i].z > topLeft.z)
                topLeft = framePosition[i];
            //top left
        }else if(framePosition[i].y > 0.5f && framePosition[i].z > 0.5f ){
            if(framePosition[i].z > topRight.z)
                topRight = framePosition[i];
            //top right
        }else if(framePosition[i].y < 0.5f && framePosition[i].z < 0.5f ){
            if(framePosition[i].z < bottomLeft.z)
                bottomLeft = framePosition[i];

            //bottom left
        }else if(framePosition[i].y > 0.5f && framePosition[i].z < 0.5f ){
            if(framePosition[i].z < bottomRight.z)
                bottomRight = framePosition[i];
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


void DICOMRenderer::calculateElectrodeMatices(){
    //left
    Vec3f to,from;
    float elecLength,dot,cross;
    Mat4f Fm,G,U,S,T;

    to = _data->getLeftSTN()._endWorldSpace-_data->getLeftSTN()._startWorldSpace;
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

    Vec3f outdir = ( _data->getLeftSTN()._startWorldSpace-_data->getLeftSTN()._endWorldSpace);
    outdir.normalize();
    T.Translation(_data->getLeftSTN()._startWorldSpace+  outdir*15.0f );

    _electrodeLeftMatrix = S*U;
    //LEFTEND;
    to = _data->getRightSTN()._endWorldSpace-_data->getRightSTN()._startWorldSpace;
    elecLength = to.length()+5;
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
    outdir = ( _data->getRightSTN()._startWorldSpace-_data->getRightSTN()._endWorldSpace);
    outdir.normalize();
    T.Translation(_data->getRightSTN()._startWorldSpace+  outdir*15.0f  );

    _electrodeRightMatix = S*U;
}

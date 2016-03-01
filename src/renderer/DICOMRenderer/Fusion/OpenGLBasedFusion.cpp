#include "OpenGLBasedFusion.h"

using namespace std;


OpenGLFusion::OpenGLFusion(std::shared_ptr<DataHandle> d, Vec2ui WindowResolution, uint32_t CTTextureHandle, uint32_t MRTextureHandle):
iFusion(d),
_windowSize(Vec2ui(0,0)),
_windowSizeReal(Vec2ui(0,0)),
_ctHandle(CTTextureHandle),
_mrHandle(MRTextureHandle){
    init(WindowResolution);
}

OpenGLFusion::~OpenGLFusion(){

}

int32_t OpenGLFusion::executeFusionStep(){
    _baseTranslation = _dataset->getMROffset();
    _baseRotation =  _dataset->getMRRotation();

    std::vector<float> subValues;

    //get currentvalue;
    _matrices.clear();
    _matrices.push_back(_dataset->getMRWorld().inverse());
    if(_lastSubstraction == -1){
        _lastSubstraction = doGD()[0];
    }
    _matrices.clear();

    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(_dataset->getFTranslationStep(),0,0)));
    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(-_dataset->getFTranslationStep(),0,0)));
    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(0,_dataset->getFTranslationStep(),0)));
    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(0,-_dataset->getFTranslationStep(),0)));
    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(0,0,_dataset->getFTranslationStep())));
    _matrices.push_back(createTranslationOffsetMatrix(Vec3f(0,0,-_dataset->getFTranslationStep())));

    _dataset->setMROffset(_baseTranslation);

    //same for rotation
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(_dataset->getFRotationStep(),0,0)));
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(-_dataset->getFRotationStep(),0,0)));
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(0,_dataset->getFRotationStep(),0)));
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(0,-_dataset->getFRotationStep(),0)));
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(0,0,_dataset->getFRotationStep())));
    _matrices.push_back(createRotationOffsetMatrix(Vec3f(0,0,-_dataset->getFRotationStep())));

    _dataset->setMRRotation(_baseRotation);

    //MRMatrixList : {ori, xtP, xtM, ytP, ytM, ztP, ztM, xrP, xrM, yrP, yrM, zrP, zrM }

    //subValues = subVolumesV2(MRMatrixList);
    subValues = doGD();

    //iterate over the vector to find the smalles substraction value
    float minSubstraction = std::abs(subValues[0]);
    int bestIndex = 0;
    for(int i = 1; i < subValues.size();++i){
        if(minSubstraction > std::abs(subValues[i])){
            minSubstraction = std::abs(subValues[i]);
            bestIndex = i;
        }
    }

    //IFF the substraction is better after any translation we have to continue
    //no minimum found!
    if(std::abs(minSubstraction) < std::abs(_lastSubstraction) ){
        _lastSubstraction = minSubstraction;
        switch(bestIndex){
            case 0: _dataset->setMROffset(_baseTranslation+Vec3f(_dataset->getFTranslationStep(),0,0)); break;
            case 1: _dataset->setMROffset(_baseTranslation+Vec3f(-_dataset->getFTranslationStep(),0,0)); break;
            case 2: _dataset->setMROffset(_baseTranslation+Vec3f(0,_dataset->getFTranslationStep(),0)); break;
            case 3: _dataset->setMROffset(_baseTranslation+Vec3f(0,-_dataset->getFTranslationStep(),0)); break;
            case 4: _dataset->setMROffset(_baseTranslation+Vec3f(0,0,_dataset->getFTranslationStep())); break;
            case 5: _dataset->setMROffset(_baseTranslation+Vec3f(0,0,-_dataset->getFTranslationStep())); break;

            case 6: _dataset->setMRRotation(_baseRotation+Vec3f(_dataset->getFRotationStep(),0,0));break;
            case 7: _dataset->setMRRotation(_baseRotation+Vec3f(-_dataset->getFRotationStep(),0,0));break;
            case 8: _dataset->setMRRotation(_baseRotation+Vec3f(0,_dataset->getFRotationStep(),0));break;
            case 9: _dataset->setMRRotation(_baseRotation+Vec3f(0,-_dataset->getFRotationStep(),0));break;
            case 10: _dataset->setMRRotation(_baseRotation+Vec3f(0,0,_dataset->getFRotationStep()));break;
            case 11: _dataset->setMRRotation(_baseRotation+Vec3f(0,0,-_dataset->getFRotationStep()));break;
        }
        return 1.0f; // 1 = continue
    }
    _lastSubstraction = -1;
    return -1.0f; // -1 finish
}

std::vector<float> OpenGLFusion::doGD(){
    //render entry of CT

    //variables
    Core::Math::Mat4f ortho;
    Core::Math::Mat4f view;
    std::vector<float> values;

    //read pixeldata!
    std::vector<Vec4f>              dataBuffer1;
    std::vector<Vec4f>              dataBuffer2;
    std::vector<Vec4f>              dataBuffer3;
    std::vector<Vec4f>              dataBuffer4;
    std::vector<Vec4f>              dataBuffer5;
    Core::Math::Vec2ui              realWindowSize;


    //initialization
    glViewport(0,0,_windowSize.x,_windowSize.y);
    ortho.Ortho(-(int32_t)_windowSize.x/2,(int32_t)_windowSize.x/2,-(int32_t)_windowSize.y/2,(int32_t)_windowSize.y/2,-5000.0f,5000.0f);
    view.BuildLookAt(Vec3f(0,200,0),Vec3f(0,0,0),Vec3f(0,0,-1));
    values.resize(_matrices.size());
    for(int i = 0; i <  values.size();++i){
         values[i] = 0;
    }

    //------- Raycaster Rendering
    _targetBinder->Bind(_rayEntryBuffer);
    glClearColor(0,0,0,0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);


    _frontFaceShader->Enable();
    _frontFaceShader->Set("projectionMatrix",ortho);
    _frontFaceShader->Set("viewMatrix",view);
    _frontFaceShader->Set("worldMatrix",_dataset->getCTWorld());

    _volumeBox->paint();

    _frontFaceShader->Disable();

    _targetBinder->Unbind();


    //render backfaces
    _targetBinder->Bind(_raySubBuffer1,_raySubBuffer2,_raySubBuffer3,_raySubBuffer4);
    glClearColor(0,0,0,0);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    _subtractRayCastShader->Enable();
    _subtractRayCastShader->Set("projectionMatrix",ortho);
    _subtractRayCastShader->Set("viewMatrix",view);
    _subtractRayCastShader->Set("worldMatrix",_dataset->getCTWorld());

    _subtractRayCastShader->SetTexture2D("rayStartPoint",_rayEntryBuffer->GetTextureHandle(),0);
    _subtractRayCastShader->SetTexture3D("CTVolume",_ctHandle,1);
    _subtractRayCastShader->SetTexture3D("MRVolume",_mrHandle,2);

    _subtractRayCastShader->Set("worldFragmentMatrix",_dataset->getCTWorld());
    _subtractRayCastShader->Set("worldMRinverseFragmentMatrix",_matrices);
    _subtractRayCastShader->Set("MRmatricies",(int32_t)_matrices.size());

    _subtractRayCastShader->Set("CTScaling",_dataset->getCTTransferScaling());
    _subtractRayCastShader->Set("MRScaling",_dataset->getMRTransferScaling());

    _subtractRayCastShader->Set("stepSize",1.0f/512.0f);

    _volumeBox->paint();

    _subtractRayCastShader->Disable();
    _targetBinder->Unbind();

    dataBuffer1.resize(_windowSize.x*_windowSize.y);
    _targetBinder->Bind(_raySubBuffer1);
    glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, _windowSize.x, _windowSize.y, GL_RGBA, GL_FLOAT, (GLvoid*)&(dataBuffer1[0]));

    if(values.size() > 1){
        dataBuffer2.resize(_windowSize.x*_windowSize.y);
        dataBuffer3.resize(_windowSize.x*_windowSize.y);
        dataBuffer4.resize(_windowSize.x*_windowSize.y);
        dataBuffer5.resize(_windowSize.x*_windowSize.y);

        _targetBinder->Bind(_raySubBuffer2);
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, _windowSize.x, _windowSize.y, GL_RGBA, GL_FLOAT, (GLvoid*)&(dataBuffer2[0]));


        _targetBinder->Bind(_raySubBuffer3);
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, _windowSize.x, _windowSize.y, GL_RGBA, GL_FLOAT, (GLvoid*)&(dataBuffer3[0]));


        _targetBinder->Bind(_raySubBuffer4);
        glReadBuffer((GLenum)GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, _windowSize.x, _windowSize.y, GL_RGBA, GL_FLOAT, (GLvoid*)&(dataBuffer4[0]));
    }

    for(int x = 0; x < dataBuffer1.size();++x){
        values[0] += dataBuffer1[x].x;

        if(values.size() > 1){
            values[1] += dataBuffer1[x].y;
            values[2] += dataBuffer1[x].z;


            values[3] += dataBuffer2[x].x;
            values[4] += dataBuffer2[x].y;
            values[5] += dataBuffer2[x].z;

            values[6] += dataBuffer3[x].x;
            values[7] += dataBuffer3[x].y;
            values[8] += dataBuffer3[x].z;

            values[9] += dataBuffer4[x].x;
            values[10] += dataBuffer4[x].y;
            values[11] += dataBuffer4[x].z;

            values[12] += dataBuffer5[x].x;
        }
    }

    glViewport(0,0,_windowSizeReal.x,_windowSizeReal.y);
    return values;
}


bool OpenGLFusion::init(Core::Math::Vec2ui resolution){
    if(_subtractRayCastShader == nullptr || _frontFaceShader == nullptr){
        //loadShader
        vector<string> fs,vs;
        vs.push_back("Shader/RayCasterVertex.glsl");
        fs.push_back("Shader/RayCasterSubtractFragment.glsl");
        ShaderDescriptor sd(vs,fs);
        _subtractRayCastShader = std::make_shared<GLProgram>();
        _subtractRayCastShader->Load(sd);
        if (!_subtractRayCastShader->IsValid()){
            cout << "programm not valid" << endl;
            return false;
        }

        vs.clear();
        fs.clear();
        vs.push_back("Shader/CubeVertex.glsl");
        fs.push_back("Shader/CubeFragment.glsl");
         sd = ShaderDescriptor(vs,fs);
        _frontFaceShader = std::make_shared<GLProgram>();
        _frontFaceShader->Load(sd);
        if (!_frontFaceShader->IsValid()){
            cout << "programm not valid" << endl;
            return false;
        }
    }

    //load basic cube geometry
    if(_volumeBox == nullptr)
        _volumeBox = std::unique_ptr<GLVolumeBox>(new GLVolumeBox());

    //creating a targetBinder to simplify switching between framebuffers
    if(_targetBinder == nullptr)
        _targetBinder = std::unique_ptr<GLTargetBinder>(new GLTargetBinder());


    //create framebuffer objects
    if(_windowSizeReal.x !=  resolution.x || _windowSizeReal.y != resolution.y){
       _windowSizeReal = resolution;
       _windowSize = Vec2ui(250,250);
       _rayEntryBuffer = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
       _raySubBuffer1  = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
       _raySubBuffer2  = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
       _raySubBuffer3  = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
       _raySubBuffer4  = std::make_shared<GLFBOTex>(GL_NEAREST, GL_NEAREST, GL_CLAMP, _windowSize.x, _windowSize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT, true, 1);
    }
    return true;
}

#include <stdio.h>
#include <iostream>
#include <core/Time/Timer.h>

#include "CudaFusion.h"

#include "CudaKernel.h"



using namespace Core::Time;

CudaFusion::CudaFusion(std::shared_ptr<DataHandle> d):
iFusion(d){
init();
}

CudaFusion::~CudaFusion(){
}


bool CudaFusion::init(Core::Math::Vec2ui resolution){
    initCuda();

    //generate textures
    unsigned short *ctPtr = &((unsigned short)_dataset->getCTData()[0]);
    unsigned short *mrPtr = &((unsigned short)_dataset->getMRData()[0]);
    generateCudaTexture(mrPtr,_dataset->getMRDimensions().x,_dataset->getMRDimensions().y,_dataset->getMRDimensions().z,false);
    generateCudaTexture(ctPtr,_dataset->getCTDimensions().x,_dataset->getCTDimensions().y,_dataset->getCTDimensions().z,true);


    executeLoop();

    return true;
}

void CudaFusion::executeLoop(){
    Timer t;

    bool cont = true;
    t.start();
    while(cont){
        if(executeFusionStep() < 0){
            if(_dataset->getFTranslationStep() < 0.5f){
                double d = t.elapsed();
                cont = false;
                std::cout << "end cuda gdc : " << d<<std::endl;
            }else{
                _dataset->setFTranslationStep(_dataset->getFTranslationStep()*_dataset->getFTranslationStepScale());
                _dataset->setFRotationStep(_dataset->getFRotationStep()*_dataset->getFRotationStepScale());
                std::cout << _dataset->getMROffset() << std::endl;
                std::cout << _dataset->getMRRotation() << std::endl;
                std::cout << "decrese step size now"<<std::endl;

            }
        }
    }

    std::cout << _dataset->getMROffset() << std::endl;
    std::cout << _dataset->getMRRotation() << std::endl;
}

int32_t CudaFusion::executeFusionStep(){
    //test some cuda stuff

        Core::Math::Vec3f baseOffset = _dataset->getMROffset();
        Core::Math::Vec3f baseRotation = _dataset->getMRRotation();


        float translationStep = _dataset->getFTranslationStep();
        float rotationStep = _dataset->getFRotationStep();

        std::vector<Core::Math::Mat4f>  __stepMatrices;
        std::vector<double> subValues;

        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());

        _dataset->setMROffset(baseOffset+Vec3f(translationStep,0,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset+Vec3f(-translationStep,0,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset+Vec3f(0.0f,translationStep,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset+Vec3f(0.0f,-translationStep,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset+Vec3f(0,0,translationStep));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset+Vec3f(0,0,-translationStep));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMROffset(baseOffset);

        _dataset->setMRRotation(baseRotation+Vec3f(rotationStep,0,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation+Vec3f(-rotationStep,0,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation+Vec3f(0,rotationStep,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation+Vec3f(0,-rotationStep,0));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation+Vec3f(0,0,rotationStep));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation+Vec3f(0,0,-rotationStep));
        __stepMatrices.push_back(_dataset->getCTWorld()*_dataset->getMRWorld().inverse());
        _dataset->setMRRotation(baseRotation);

        //ONE VOLUME SUB START
        setEmptyResultVector(__stepMatrices.size());
        setMatrixVector(&(__stepMatrices[0])[0],
                          __stepMatrices.size());

        subValues = subtractVolume(_dataset->getCTDimensions().x,
                                                       _dataset->getCTDimensions().y,
                                                       _dataset->getCTDimensions().z);
        __stepMatrices.clear();
        //ONE VOLUME SUB END


       float _lastSubstraction = subValues[0];

        //iterate over the vector to find the smalles substraction value
        float minSubstraction = std::abs(subValues[1]);
        int bestIndex = 1;
        for(int i = 2; i < subValues.size();++i){
            if(minSubstraction > std::abs(subValues[i])){
                minSubstraction = std::abs(subValues[i]);
                bestIndex = i;
            }
        }

        //IFF the substraction is better after any translation we have to continue
        //no minimum found!
        if(std::abs(minSubstraction) < std::abs(_lastSubstraction) ){
            //std::cout <<"bestIndex "<< bestIndex << std::endl;
            _lastSubstraction = minSubstraction;
            switch(bestIndex){
                case 1: _dataset->setMROffset(baseOffset+Vec3f(translationStep,0,0)); break;
                case 2: _dataset->setMROffset(baseOffset+Vec3f(-translationStep,0,0)); break;
                case 3: _dataset->setMROffset(baseOffset+Vec3f(0,translationStep,0)); break;
                case 4: _dataset->setMROffset(baseOffset+Vec3f(0,-translationStep,0)); break;
                case 5: _dataset->setMROffset(baseOffset+Vec3f(0,0,translationStep)); break;
                case 6: _dataset->setMROffset(baseOffset+Vec3f(0,0,-translationStep)); break;

                case 7: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(rotationStep,0,0));break;
                case 8: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(-rotationStep,0,0));break;
                case 9: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(0,rotationStep,0));break;
                case 10: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(0,-rotationStep,0));break;
                case 11: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(0,0,rotationStep));break;
                case 12: _dataset->setMRRotation(_dataset->getMRRotation()+Vec3f(0,0,-rotationStep));break;
            }
            return 1.0f;
        }
    return -1.0f;
}

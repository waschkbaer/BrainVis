#include <stdio.h>
#include <iostream>
#include <core/Time/Timer.h>

#include "CudaFusion.h"

#include "CudaKernel.h"



using namespace Core::Time;

CudaFusion::CudaFusion(std::shared_ptr<DataHandle> d):
iFusion(d),
translationStepSize(8.0f),
rotationStepSize(0.01f),
ScalingOnMiss(0.61f),
m_CTDimensions(0,0,0){
}

CudaFusion::~CudaFusion(){
}


bool CudaFusion::init(Core::Math::Vec2ui resolution){
    m_CTDimensions = _dataset->getCTDimensions();
    initCuda();

    //generate textures
    /*unsigned short *ctPtr = &((unsigned short)_dataset->getCTData()[0]);
    unsigned short *mrPtr = &((unsigned short)_dataset->getMRData()[0]);
    generateCudaTexture(mrPtr,_dataset->getMRDimensions().x,_dataset->getMRDimensions().y,_dataset->getMRDimensions().z,false);
    generateCudaTexture(ctPtr,m_CTDimensions.x,m_CTDimensions.y,m_CTDimensions.z,true);
*/
    vm_stepMatrices.resize(13);
    initDevice(m_CTDimensions.x/2,m_CTDimensions.y/2,m_CTDimensions.z/2);
    updateCTMatrix();

    //execute();
    //executeFusionStep();

    return true;
}

void CudaFusion::execute(){
    bool cont = true; //continue=

       while(cont){
           // if the result of the loop is negative the step has not found
           // any better postion for the mr value
           values = executeFusionStep(values);
           if(!values._changed){

               //if the translationstep is smaller then a certain number (0.5f)
               //i will stop the loop
               if(translationStepSize < 0.5f){
                   cont = false;
               //else i will decrement the step size and continue looking
               //for a local minimum around the current position
               }else{
                   translationStepSize = translationStepSize * ScalingOnMiss;
                   rotationStepSize = rotationStepSize * ScalingOnMiss;
               }
           }
       }

       //result
       _dataset->setMROffset(values._translation);
       _dataset->setMRRotation(values._rotation);
}

transformationTupel  CudaFusion::executeFusionStep(transformationTupel input){
        std::vector<transformationTupel> outputs;
        outputs.push_back(transformationTupel(input._translation,input._rotation));

        //current position
        updateMRMatrix(outputs[0]._translation,outputs[0]._rotation);
        vm_stepMatrices[0] = m_CTMatrix*m_MRMatrix.inverse();

        //COPY PASTE HORROR!! creating matrices for each axis translation rotation

        //translation +x -x +y -y +z -z
        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(translationStepSize,0,0),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[1]._translation,outputs[1]._rotation);
        vm_stepMatrices[1] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(-translationStepSize,0,0),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[2]._translation,outputs[2]._rotation);
        vm_stepMatrices[2] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(0,translationStepSize,0),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[3]._translation,outputs[3]._rotation);
        vm_stepMatrices[3] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(0,-translationStepSize,0),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[4]._translation,outputs[4]._rotation);
        vm_stepMatrices[4] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(0,0,translationStepSize),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[5]._translation,outputs[5]._rotation);
        vm_stepMatrices[5] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation+Core::Math::Vec3f(0,0,-translationStepSize),
                                              input._rotation,
                                              true));
        updateMRMatrix(outputs[6]._translation,outputs[6]._rotation);
        vm_stepMatrices[6] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(rotationStepSize,0,0),
                                              true));
        updateMRMatrix(outputs[7]._translation,outputs[7]._rotation);
        vm_stepMatrices[7] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(-rotationStepSize,0,0),
                                              true));
        updateMRMatrix(outputs[8]._translation,outputs[8]._rotation);
        vm_stepMatrices[8] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(0,rotationStepSize,0),
                                              true));
        updateMRMatrix(outputs[9]._translation,outputs[9]._rotation);
        vm_stepMatrices[9] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(0,-rotationStepSize,0),
                                              true));
        updateMRMatrix(outputs[10]._translation,outputs[10]._rotation);
        vm_stepMatrices[10] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(0,0,rotationStepSize),
                                              true));
        updateMRMatrix(outputs[11]._translation,outputs[11]._rotation);
        vm_stepMatrices[11] = m_CTMatrix*m_MRMatrix.inverse();

        outputs.push_back(transformationTupel(input._translation,
                                              input._rotation+Core::Math::Vec3f(0,0,-rotationStepSize),
                                              true));
        updateMRMatrix(outputs[12]._translation,outputs[12]._rotation);
        vm_stepMatrices[12] = m_CTMatrix*m_MRMatrix.inverse();


        //timer2.start();
        std::vector<float> substractionResults = step(m_CTDimensions.x/2,m_CTDimensions.y/2,m_CTDimensions.z/2,&(vm_stepMatrices[0])[0]);
        //double el = timer2.elapsed();
        //std::cout << "time sub: " << el << std::endl;

        float current = substractionResults[0];

        //iterate over the vector to find the smalles substraction value
        float minSubstraction = std::abs(substractionResults[1]);
        int bestIndex = 1;
        for(int i = 2; i < substractionResults.size();++i){
            if(minSubstraction > std::abs(substractionResults[i])){
                minSubstraction = std::abs(substractionResults[i]);
                bestIndex = i;
            }
        }


        if(minSubstraction <  current ){
            return outputs[bestIndex];
        }
        //if the step returns -1 the loop will decrementt the step size*/
        return outputs[0];
}

void CudaFusion::updateMRMatrix(const Core::Math::Vec3f& translation,const Core::Math::Vec3f& rotation){
    Core::Math::Mat4f scale;
    Core::Math::Mat4f trans;
    Core::Math::Mat4f rotX,rotY,rotZ;

    scale.Scaling(_dataset->getMRScale());
    trans.Translation(translation);
    rotX.RotationX(rotation.x);
    rotY.RotationY(rotation.y);
    rotZ.RotationZ(rotation.z);

    m_MRMatrix = scale*rotX*rotY*rotZ*trans;
}

void CudaFusion::updateCTMatrix(){
    m_CTMatrix.Scaling(_dataset->getCTScale());
}

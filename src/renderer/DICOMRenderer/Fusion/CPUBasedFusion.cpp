#include "CPUBasedFusion.h"

CPUFusion::CPUFusion(std::shared_ptr<DataHandle> dataset):
iFusion(dataset){}
CPUFusion::~CPUFusion(){}

bool CPUFusion::init(Core::Math::Vec2ui resolution){

    return true;
}
int32_t CPUFusion::executeFusionStep(){
    _baseTranslation = _dataset->getMROffset();
    _baseRotation =  _dataset->getMRRotation();

    std::vector<float> subValues;

    //get currentvalue;
    if(_lastSubstraction == -1){
        _matrices.clear();
        _matrices.push_back(_dataset->getMRWorld().inverse());

        _results.clear();
        _results.resize(_matrices.size());
        for(int i = 0; i < _results.size();++i){
            _results[i] = 0;
        }
        doGDThreads();
        _lastSubstraction = _results[0];
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
    _results.clear();
    _results.resize(_matrices.size());
    for(int i = 0; i < _results.size();++i){
        _results[i] = 0;
    }
    doGDThreads();
    subValues = _results;

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
    return -1.0f; // -1 finish
}

void CPUFusion::doGDThreads(){
    int N = 8;
    std::vector<std::thread*> threads;

    for(int i = 0; i < N;++i){
        threads.push_back(new std::thread(&CPUFusion::doGD,this,(double)i));
    }

    threads[0]->join();
    threads[1]->join();
    threads[2]->join();
    threads[3]->join();
    threads[4]->join();
    threads[5]->join();
    threads[6]->join();
    threads[7]->join();
}

void CPUFusion::doGD(double threadIndex){
    Vec3ui ctDim = _dataset->getCTDimensions();
    Vec3d ctSteps = Vec3d(1.0/(double)(ctDim.x-1) , 1.0/(double)(ctDim.y-1) , 1.0/(double)(ctDim.z-1));
    Vec3f CTVolPos;
    Vec4f MRVolPos;

    float ctValue = 0;
    float mrValue = 0;

    float ctMaxValue = _dataset->getCTHistogramm().size();
    float mrMaxValue = _dataset->getMRHistogramm().size();

    double startx = threadIndex/8.0f;
    double endx   = startx + 0.124999999999f;

    std::vector<float> vals;
    vals.resize(_matrices.size());
    for(int i= 0; i < vals.size();++i){
        vals[i] = 0;
    }

    for(double x = startx; x <= endx; x+= ctSteps.x){
        for(double y = 0; y <= 1.0; y += ctSteps.y){
            for(double z = 0; z <= 1.0; z += ctSteps.z){
                mrValue = 0;
                ctValue = 0;

                CTVolPos = Vec3f(x,y,z);

                CTVolPos.x = std::max(0.0f, std::min(CTVolPos.x, 1.0f));
                CTVolPos.y = std::max(0.0f, std::min(CTVolPos.y, 1.0f));
                CTVolPos.z = std::max(0.0f, std::min(CTVolPos.z, 1.0f));

                ctValue = (float)_dataset->getCTValue(CTVolPos)/ctMaxValue;
                if(ctValue > 0.6f){
                    ctValue = 0;
                }

                for(int i = 0; i < _matrices.size();++i){
                    mrValue = 0;
                    MRVolPos = _dataset->getCTWorld() * Vec4f(CTVolPos.x-0.5f,CTVolPos.y-0.5f,CTVolPos.z-0.5f,1) ;
                    MRVolPos = MRVolPos*_matrices[i];
                    MRVolPos.x += 0.5f;
                    MRVolPos.y += 0.5f;
                    MRVolPos.z += 0.5f;


                    if( MRVolPos.x >= 0.0f && MRVolPos.y >= 0.0f && MRVolPos.z >= 0.0f &&
                        MRVolPos.y <= 1.0f && MRVolPos.y <= 1.0f && MRVolPos.z <= 1.0f){
                        mrValue = (float)_dataset->getMRValue(MRVolPos.xyz())/mrMaxValue;
                    }

                    vals[i] += (ctValue-mrValue)*(ctValue-mrValue);
                }
            }
        }
    }
    _resultLock.lock();
    for(int i = 0; i < vals.size();++i){
        _results[i] += vals[i];
    }
    _resultLock.unlock();
}

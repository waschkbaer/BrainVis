#ifndef IFUSIONINTERFACE
#define IFUSIONINTERFACE

#include <core/Math/Vectors.h>
#include <vector>
#include <memory>

#include <io/DataHandle.h>

class iFusion{
public:
    iFusion():_lastSubstraction(-1),_dataset(nullptr){};
    iFusion(std::shared_ptr<DataHandle> d):_lastSubstraction(-1),_dataset(d){};

    void setDataSet(std::shared_ptr<DataHandle> dataset){
        _dataset = dataset;
    }

    void reset() { _lastSubstraction = -1; }
    virtual bool init(Core::Math::Vec2ui resolution = Vec2ui(0,0)) = 0;

    virtual int32_t executeFusionStep() = 0;

    virtual void execute() = 0;

protected:
    Core::Math::Mat4f createTranslationOffsetMatrix(Vec3f dir){
        _dataset->setMROffset(_baseTranslation+dir);
        return _dataset->getMRWorld().inverse();
    }
    Core::Math::Mat4f createRotationOffsetMatrix(Vec3f rot){
        _dataset->setMRRotation(_baseRotation+rot);
        return _dataset->getMRWorld().inverse();
    }


protected:
    double                          _lastSubstraction;
    std::shared_ptr<DataHandle>     _dataset;
    Core::Math::Vec3f               _baseTranslation;
    Core::Math::Vec3f               _baseRotation;
};

#endif //IFUSIONINTERFACE

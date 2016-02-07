#ifndef IFUSIONINTERFACE
#define IFUSIONINTERFACE

#include <core/Math/Vectors.h>
#include <vector>
#include <memory>

#include <BrainVisIO/DataHandle.h>

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

protected:
    double                          _lastSubstraction;
    std::shared_ptr<DataHandle>     _dataset;
};

#endif //IFUSIONINTERFACE

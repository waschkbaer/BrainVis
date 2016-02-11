#ifndef CUDAFusion
#define CUDAFusion

#include "iFusion.h"

class CudaFusion: public iFusion{
public:
    CudaFusion(std::shared_ptr<DataHandle> d);
    ~CudaFusion();

    bool init(Core::Math::Vec2ui resolution = Core::Math::Vec2ui(0,0));
    int32_t executeFusionStep();

    void executeLoop();

protected:
private:
    unsigned short*                         _CTVolumePtr;
    unsigned short*                         _MRVolumePtr;
    std::vector<float*>                     _matricesPtr;
    std::vector<Core::Math::Mat4f>          __stepMatrices;

};

#endif CUDAFusion

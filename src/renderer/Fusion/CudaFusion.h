#ifndef CUDAFusion
#define CUDAFusion

#include "iFusion.h"

struct transformationTupel{

    transformationTupel(Core::Math::Vec3f translation, Core::Math::Vec3f rotation):
        _translation(translation),
        _rotation(rotation),
        _changed(false)
    {};
    transformationTupel(Core::Math::Vec3f translation, Core::Math::Vec3f rotation,bool changed):
        _translation(translation),
        _rotation(rotation),
        _changed(changed)
    {};
    transformationTupel():
        _translation(0,0,0),
        _rotation(0,0,0),
        _changed(false)
    {};

    Core::Math::Vec3f _translation;
    Core::Math::Vec3f _rotation;
    bool              _changed;
};

class CudaFusion: public iFusion{
public:
    CudaFusion(std::shared_ptr<DataHandle> d);
    ~CudaFusion();

    bool init(Core::Math::Vec2ui resolution = Core::Math::Vec2ui(0,0));
    transformationTupel executeFusionStep(transformationTupel input);
    int32_t executeFusionStep(){return 0;};

    void execute();

protected:
private:
    unsigned short*                         _CTVolumePtr;
    unsigned short*                         _MRVolumePtr;
    std::vector<float*>                     _matricesPtr;
    std::vector<Core::Math::Mat4f>          vm_stepMatrices;

    Core::Math::Mat4f                       m_CTMatrix;
    Core::Math::Mat4f                       m_MRMatrix;
    Core::Math::Vec3ui                       m_CTDimensions;
    transformationTupel                     values;

    float                                   translationStepSize;
    float                                   rotationStepSize;
    float                                   ScalingOnMiss;

    void updateMRMatrix(const Core::Math::Vec3f& translation,const Core::Math::Vec3f& rotation);
    void updateCTMatrix();

};

#endif CUDAFusion

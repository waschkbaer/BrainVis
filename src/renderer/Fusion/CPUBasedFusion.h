#ifndef CPUFUSION
#define CPUFUSION


#include "iFusion.h"
#include <thread>
#include <mutex>

class CPUFusion: public iFusion{
public:
    CPUFusion(std::shared_ptr<DataHandle> dataset);
    ~CPUFusion();

    bool init(Core::Math::Vec2ui resolution = Vec2ui(0,0));
    int32_t executeFusionStep();
    void execute(){};

private:
    void doGDThreads();

    void doGD(double threadIndex);
private:
    std::vector<Core::Math::Mat4f>  _matrices;
    std::vector<float>              _results;
    std::mutex                      _resultLock;
};

#endif //CPUFUSION

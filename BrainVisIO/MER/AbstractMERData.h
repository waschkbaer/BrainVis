#ifndef ABSTRMER_DATA
#define ABSTRMER_DATA

#include <core/Math/Vectors.h>
#include <vector>
#include <memory>
#include <string>

class MERData{
public:
    MERData();
    MERData(int32_t depth, Core::Math::Vec3f position,
            float spectralPower, std::vector<float> spectralFlow,
            std::vector<float> rawData);
    virtual ~MERData(){};

    int32_t depth() const;
    void setDepth(const int32_t &depth);

    float spectralPower() const;
    void setSpectralPower(float spectralPower);

    std::vector<float> spectralFlow() const;
    void setSpectralFlow(const std::vector<float> &spectralFlow);

    std::vector<float> rawData() const;
    void setRawData(const std::vector<float> &rawData);

    Core::Math::Vec3f position() const;
    void setPosition(const Core::Math::Vec3f &position);

    Core::Math::Vec2ui freqRange() const;
    void setFreqRange(const Core::Math::Vec2ui &freqRange);

    Core::Math::Vec2f spectralRange() const;
    void setSpectralRange(const Core::Math::Vec2f &spectralRange);

private:
    int32_t             _depth;
    Core::Math::Vec3f   _position;
    float               _spectralPower;
    Core::Math::Vec2f               _spectralRange;
    std::vector<float>  _spectralFlow;
    std::vector<float>  _rawData;
    Core::Math::Vec2ui  _freqRange;
};



class AbstrElectrode{
public:
    AbstrElectrode(){};
    virtual ~AbstrElectrode(){};

    virtual std::shared_ptr<MERData> getData(int32_t depth) = 0;
    virtual void setData(int32_t depth, std::shared_ptr<MERData> data) = 0;
    virtual Core::Math::Vec2i getDepthRange() = 0;

    std::string getName() const;
    void setName(const std::string &name);

    Core::Math::Vec2f getElektrodeRange() const;
    void setElektrodeRange(const Core::Math::Vec2f &elektrodeRange);

private:
    std::string _name;
    Core::Math::Vec2f _elektrodeRange;
};


class AbstrElectrodeBundle{
public:
    AbstrElectrodeBundle(){};
    virtual ~AbstrElectrodeBundle(){};

    virtual std::shared_ptr<AbstrElectrode> getTrajectory(uint32_t i = 0) = 0;
    virtual uint32_t getTrajectoryCount() = 0;
    virtual void addTrajectory(std::shared_ptr<AbstrElectrode> traj) = 0;

protected:
    std::vector<std::shared_ptr<AbstrElectrode>> _trajectories;
};


#endif //ABSTRMER_DATA

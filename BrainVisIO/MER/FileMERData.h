#ifndef FILEMER_DATA
#define FILEMER_DATA

#include "AbstractMERData.h"


class FileElectrode : public AbstrElectrode{
public:
    FileElectrode(std::string name, std::string positions, std::string spectral, Core::Math::Vec2ui range);
    ~FileElectrode();

    std::shared_ptr<MERData> getData(int32_t depth) override;
    void setData(int32_t depth, std::shared_ptr<MERData> data) override;
    Core::Math::Vec2i getDepthRange() override;

private:
    bool loadPositions(std::string positions);
    bool loadFFT(std::string spectral, Core::Math::Vec2ui range);

private:
    std::vector<std::shared_ptr<MERData>> _data;
};

class FileElectrodeBundle : public AbstrElectrodeBundle{
    public:
    FileElectrodeBundle(
            std::string name1, std::string positions1, std::string spectral1,
            std::string name2, std::string positions2, std::string spectral2,
            std::string name3, std::string positions3, std::string spectral3
            );
    FileElectrodeBundle(
            std::vector<std::string> name,
            std::vector<std::string> position,
            std::vector<std::string> spectral,
            Core::Math::Vec2ui range
            );
    ~FileElectrodeBundle();

    std::shared_ptr<AbstrElectrode> getTrajectory(uint32_t i = 0) override;
    uint32_t getTrajectoryCount() override;
    void addTrajectory(std::shared_ptr<AbstrElectrode> traj) override;
};


#endif //FILEMER_DATA

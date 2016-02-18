#ifndef BRAINVIS_MERElectrode
#define BRAINVIS_MERElectrode

#include <vector>
#include <string>
#include <map>
#include <core/Math/Vectors.h>

namespace BrainVisIO{
namespace MERData{

class MERData;

class MERElectrode{
public:
    MERElectrode(Core::Math::Vec3f targetPosition = Core::Math::Vec3f(0,0,0), int startDepth = -10);
    MERElectrode(const std::vector<std::string>& filelist, Core::Math::Vec3f targetPosition = Core::Math::Vec3f(0,0,0),  int startDepth = -10);
    ~MERElectrode();

    void newRecording();
    void calculateFFT();

    int getLatestDepth();

    const std::shared_ptr<MERData> getMERData(int depth);

    Core::Math::Vec3f getTargetPosition() const;
    void setTargetPosition(const Core::Math::Vec3f &targetPosition);

    Core::Math::Vec3f getElctrodeDirection() const;
    void setElctrodeDirection(const Core::Math::Vec3f &elctrodeDirection);

    bool getElectrodeVisible() const;
    void setElectrodeVisible(bool electrodeVisible);

protected:

private:
    void loadRecordings(const std::vector<std::string>& filelist);

    int                                     _currentDepth;
    std::map<int,std::shared_ptr<MERData>>  _data;
    Core::Math::Vec3f                       _targetPosition;
    Core::Math::Vec3f                       _elctrodeDirection;
    bool                                    _electrodeVisible;

};

}
}


#endif

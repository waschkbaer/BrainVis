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
    MERElectrode(Core::Math::Vec3f startPosition = Core::Math::Vec3f(0,0,0), int startDepth = -10);
    MERElectrode(const std::vector<std::string>& filelist, Core::Math::Vec3f startPosition = Core::Math::Vec3f(0,0,0),  int startDepth = -10);
    ~MERElectrode();

    void newRecording();
    void calculateFFT();

    int getLatestDepth();

    const std::shared_ptr<MERData> getMERData(int depth);

protected:

private:
    void loadRecordings(const std::vector<std::string>& filelist);

    int                                     _currentDepth;
    std::map<int,std::shared_ptr<MERData>>  _data;
    Core::Math::Vec3f                       _startPosition;

};

}
}


#endif

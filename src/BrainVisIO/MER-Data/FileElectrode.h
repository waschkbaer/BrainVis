#ifndef FILEELECTRODE
#define FILEELECTRODE

#include <vector>
#include <map>
#include <core/Math/Vectors.h>
#include "iMERData.h"
#include "iElectrode.h"
#include <memory>

class FileElectrode: public iElectrode{
public:
    FileElectrode(std::string name,
                  Core::Math::Vec2d range,
                  std::string positionFile,
                  std::string spectralFile);
    virtual ~FileElectrode();

    void updateElectrode();
    void recalculateFrequencyRange();

protected:

private:
    bool loadFiles(std::string Position, std::string Spectral);

};


#endif //FILEELECTRODE

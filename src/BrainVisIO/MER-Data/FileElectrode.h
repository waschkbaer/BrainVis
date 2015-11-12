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

    //return -1 if depth already stored
    int8_t addData(int8_t depth, std::shared_ptr<iMERData> data);
    std::shared_ptr<iMERData> getData(int8_t depth = 0);

protected:

private:
    bool loadFiles(std::string Position, std::string Spectral);

};


#endif //FILEELECTRODE

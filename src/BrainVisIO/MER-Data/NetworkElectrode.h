#ifndef NETWORKELECTRODE
#define NETWORKELECTRODE

#include <vector>
#include <map>
#include <core/Math/Vectors.h>
#include "iMERData.h"
#include "iElectrode.h"
#include <memory>

class NetworkElectrode: public iElectrode{
public:
    NetworkElectrode(std::string name);
    virtual ~NetworkElectrode();

    void updateElectrode();
    void recalculateFrequencyRange();

protected:

private:

};


#endif //NETWORKELECTRODE

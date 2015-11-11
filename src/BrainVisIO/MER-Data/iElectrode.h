#ifndef IELECTRODE_H
#define IELECTRODE_H

#include <vector>
#include <map>
#include <core/Math/Vectors.h>
#include "iMERData.h"

class iElectrode{
public:
    iElectrode(std::string name):
        _name(name),
        _SpectralPowerRange(Core::Math::Vec2d(100000.0,-100000.0))
    {}
    virtual ~iElectrode(){}

    //return -1 if depth already stored
    virtual int8_t addData(int8_t depth, std::shared_ptr<iMERData> data) = 0;
    virtual std::shared_ptr<iMERData> getData(int8_t depth = 0) = 0;

    Core::Math::Vec2d getSpectralPowerRange() const{
        return _SpectralPowerRange;
        }

    std::string getName() const {
        return _name;
       }

protected:
    std::map<int8_t,std::shared_ptr<iMERData>>      _electrodeData;
    std::string                                     _name;
    Core::Math::Vec2d                               _SpectralPowerRange;
    double                                          _SpectralAverage;

private:

};

#endif //IELECTRODE_H

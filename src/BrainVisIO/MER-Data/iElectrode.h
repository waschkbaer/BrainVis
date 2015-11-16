#ifndef IELECTRODE_H
#define IELECTRODE_H

#include <vector>
#include <map>
#include <core/Math/Vectors.h>
#include "iMERData.h"
#include <algorithm>

class iElectrode{
public:
    iElectrode(std::string name = "none", Core::Math::Vec2d range = Core::Math::Vec2d(0,100000.0)):
        _name(name),
        _SpectralPowerRange(Core::Math::Vec2d(100000.0,-100000.0)),
        _analysisRange(range),
        _depthRange(Core::Math::Vec2d(100000.0,-1000000.0))
    {}
    virtual ~iElectrode(){}

    //return -1 if depth already stored
    int8_t addData(int8_t depth, std::shared_ptr<iMERData> data){
        if(_electrodeData.find(depth) != _electrodeData.end()){
            return -1;
        }
        _electrodeData.insert(std::pair<int8_t,std::shared_ptr<iMERData>>(depth,data));

        _SpectralPowerRange.x = std::min(_SpectralPowerRange.x, data->getMinMaxSpextralPower().x);
        _SpectralPowerRange.y = std::max(_SpectralPowerRange.y, data->getMinMaxSpextralPower().y);

        _depthRange.x = std::min(_depthRange.x, (double)depth);
        _depthRange.y = std::max(_depthRange.y, (double)depth);
        return 1;
    }

    std::shared_ptr<iMERData> getData(int8_t depth = 0){
        if(_electrodeData.find(depth) != _electrodeData.end()){
            return _electrodeData.find(depth)->second;
        }
        return nullptr;
    }

    virtual void updateElectrode() = 0;
    virtual void recalculateFrequencyRange() = 0;

    Core::Math::Vec2d getSpectralPowerRange() const{
        return _SpectralPowerRange;
        }

    std::string getName() const {
        return _name;
       }

    Core::Math::Vec2d getAnalysisRange() const{
        return _analysisRange;
    }
    void setAnalysisRange(Core::Math::Vec2d range){
        _analysisRange = range;
    }

    Core::Math::Vec2d getDepthRange() const {
        return _depthRange;
    }

protected:
    std::map<int8_t,std::shared_ptr<iMERData>>      _electrodeData;
    std::string                                     _name;
    Core::Math::Vec2d                               _SpectralPowerRange;
    double                                          _SpectralAverage;
    Core::Math::Vec2d                               _analysisRange;
    Core::Math::Vec2d                               _depthRange;

private:

};

#endif //IELECTRODE_H

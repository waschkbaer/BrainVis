#ifndef NETWORKMERDATA
#define NETWORKMERDATA

#include <vector>
#include <core/Math/Vectors.h>
#include "iMERData.h"

class NetworkMERData : public iMERData{
public:
    NetworkMERData();
    virtual ~NetworkMERData();

    void setInput(std::vector<double> input);
    void setFrequency(double f);
    void setFrequencyRange(Core::Math::Vec2d range);
    void setDataPosition(Core::Math::Vec3f p);

    std::vector<double> getInput();
    std::vector<double> getSpectralPower();
    Core::Math::Vec2d getFrequencyRange();
    Core::Math::Vec3f getDataPosition();
    Core::Math::Vec2d getMinMaxSpextralPower();
    double getSpectralAverage();

    void executeFFT();

protected:
   std::vector<double>  _rawData; //input
   std::vector<double>  _spectralData; //calc

   double               _frequency; //input
   Core::Math::Vec2d    _range; //input!?!
   Core::Math::Vec3f    _position; //input
   Core::Math::Vec2d    _spectralMinMax; //calc
   double               _spectralAverage; //calc

private:



};


#endif //NETWORKMERDATA

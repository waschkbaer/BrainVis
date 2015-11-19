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
    Core::Math::Vec2d getInputRange(){ return _inputRange;}
    std::vector<double> getSpectralPower();
    Core::Math::Vec2d getFrequencyRange();
    Core::Math::Vec3f getDataPosition();
    Core::Math::Vec2d getMinMaxSpextralPower();
    double getSpectralAverage();

    void executeFFT();

    std::string getClassification();
    void setClassification(std::string classification);

protected:
   std::vector<double>  _rawData; //input
   Core::Math::Vec2d    _inputRange; //Calc
   std::vector<double>  _spectralData; //calc

   double               _frequency; //input
   Core::Math::Vec2d    _range; //input!?!
   Core::Math::Vec3f    _position; //input
   Core::Math::Vec2d    _spectralMinMax; //calc
   double               _spectralAverage; //calc

   std::string          _classification;

private:



};


#endif //NETWORKMERDATA

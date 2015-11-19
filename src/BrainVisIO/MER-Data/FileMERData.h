#ifndef FILEMERDATA
#define FILEMERDATA

#include <vector>
#include <core/Math/Vectors.h>
#include "iMERData.h"

class FileMERData : public iMERData{
public:
    FileMERData(std::vector<double> spectralData,
                Core::Math::Vec2d spectralMinMax,
                double spectralAverage);
    virtual ~FileMERData();

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

    std::string getClassification();
    void setClassification(std::string classification);

protected:
   std::vector<double>  _rawData;
   std::vector<double>  _spectralData;

   double               _frequency;
   Core::Math::Vec2d    _range;
   Core::Math::Vec3f    _position;
   Core::Math::Vec2d    _spectralMinMax;
   double               _spectralAverage;

   std::string          _classification;

private:

};


#endif //FILEMERDATA

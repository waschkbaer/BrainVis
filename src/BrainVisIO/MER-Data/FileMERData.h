#ifndef FILEMERDATA
#define FILEMERDATA

#include <vector>
#include <core/Math/Vectors.h>
#include "iMERData.h"

class FileMERData : public iMERData{
public:
    FileMERData();
    virtual ~FileMERData();

    void setInput(std::vector<double> input);
    void setSpectralPower(std::vector<double> power);
    void setFrequency(double f);
    void setFrequencyRange(Core::Math::Vec2d range);
    void setDataPosition(Core::Math::Vec3f p);

    std::vector<double> getInput();
    std::vector<double> getSpectralPower();
    Core::Math::Vec2d getFrequencyRange();
    Core::Math::Vec3f getDataPosition();

protected:
   std::vector<double>  _rawData;
   std::vector<double>  _spectralData;

   double               _frequency;
   Core::Math::Vec2d    _range;
   Core::Math::Vec3f    _position;

private:

};


#endif //FILEMERDATA

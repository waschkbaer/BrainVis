#ifndef IMERDATA
#define IMERDATA

#include <vector>
#include <core/Math/Vectors.h>
#include <string>

class iMERData{
public:
    iMERData(){}
    virtual ~iMERData(){}

    virtual void setInput(std::vector<double> input) = 0;
    virtual void setFrequency(double f) = 0;
    virtual void setFrequencyRange(Core::Math::Vec2d range) = 0;
    virtual void setDataPosition(Core::Math::Vec3f p) = 0;

    virtual std::vector<double> getInput() = 0;
    virtual std::vector<double> getSpectralPower() = 0;
    virtual Core::Math::Vec2d getFrequencyRange() = 0;
    virtual Core::Math::Vec3f getDataPosition() = 0;
    virtual Core::Math::Vec2d getMinMaxSpextralPower() = 0;
    virtual double getSpectralAverage() = 0;

    virtual void executeFFT() = 0;

    virtual std::string getClassification() = 0;
    virtual void setClassification(std::string classification) = 0;

protected:

private:


};


#endif //IMERDATA

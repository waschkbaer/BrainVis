#include "AbstractMERData.h"

MERData::MERData(){}
MERData::MERData(int32_t depth, Core::Math::Vec3f position,
        float spectralPower, std::vector<float> spectralFlow,
        std::vector<float> rawData):
    _depth(depth),
    _position(position),
    _spectralPower(spectralPower),
    _spectralFlow(spectralFlow),
    _rawData(rawData)
{

}


int32_t MERData::depth() const
{
    return _depth;
}

void MERData::setDepth(const int32_t &depth)
{
    _depth = depth;
}

float MERData::spectralPower() const
{
    return _spectralPower;
}

void MERData::setSpectralPower(float spectralPower)
{
    _spectralPower = spectralPower;
}

std::vector<float> MERData::spectralFlow() const
{
    return _spectralFlow;
}

void MERData::setSpectralFlow(const std::vector<float> &spectralFlow)
{
    _spectralFlow = spectralFlow;
}

std::vector<float> MERData::rawData() const
{
    return _rawData;
}

void MERData::setRawData(const std::vector<float> &rawData)
{
    _rawData = rawData;
}

Core::Math::Vec3f MERData::position() const
{
    return _position;
}

void MERData::setPosition(const Core::Math::Vec3f &position)
{
    _position = position;
}
Core::Math::Vec2ui MERData::freqRange() const
{
    return _freqRange;
}

void MERData::setFreqRange(const Core::Math::Vec2ui &freqRange)
{
    _freqRange = freqRange;
}
Core::Math::Vec2f MERData::spectralRange() const
{
    return _spectralRange;
}

void MERData::setSpectralRange(const Core::Math::Vec2f &spectralRange)
{
    _spectralRange = spectralRange;
}



std::string AbstrElectrode::getName() const
{
    return _name;
}

void AbstrElectrode::setName(const std::string &name)
{
    _name = name;
}
Core::Math::Vec2f AbstrElectrode::getElektrodeRange() const
{
    return _elektrodeRange;
}

void AbstrElectrode::setElektrodeRange(const Core::Math::Vec2f &elektrodeRange)
{
    _elektrodeRange = elektrodeRange;
}


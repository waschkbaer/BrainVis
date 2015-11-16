#include "NetworkMERData.h"
#include <BrainVisIO/FFT/FFT.h>

NetworkMERData::NetworkMERData():
_spectralMinMax(10000000.0f,-10000000.0f){

}

NetworkMERData::~NetworkMERData(){

}

void NetworkMERData::setInput(std::vector<double> input){
    _rawData = input;
}


void NetworkMERData::setFrequency(double f){
    _frequency = f;
}

void NetworkMERData::setFrequencyRange(Core::Math::Vec2d range){
    _range = range;
}

void NetworkMERData::setDataPosition(Core::Math::Vec3f p){
    _position = p;
}

std::vector<double> NetworkMERData::getInput(){
    return _rawData;
}

std::vector<double> NetworkMERData::getSpectralPower(){
    return _spectralData;
}

Core::Math::Vec2d NetworkMERData::getFrequencyRange(){
    return _range;
}

Core::Math::Vec3f NetworkMERData::getDataPosition(){
    return _position;
}

Core::Math::Vec2d NetworkMERData::getMinMaxSpextralPower(){
    return _spectralMinMax;
}

double NetworkMERData::getSpectralAverage(){
    return _spectralAverage;
}

void NetworkMERData::executeFFT(){
    FFTCalc calc;
    calc.setInput(_rawData);
    calc.execute();
    _spectralData = calc.getSpectrum();

    double stepsize = _frequency / (double)_spectralData.size();

    double specAvg = 0;
    for(int i = _range.x/stepsize; i < _range.y/stepsize;++i){
        if(_spectralMinMax.x > _spectralData[i]){
            _spectralMinMax.x = _spectralData[i];
        }
        if(_spectralMinMax.y < _spectralData[i]){
            _spectralMinMax.y = _spectralData[i];
        }
        specAvg += _spectralData[i];
    }
    specAvg /= _spectralData.size();
}

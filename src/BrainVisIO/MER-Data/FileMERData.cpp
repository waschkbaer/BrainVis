#include "FileMERData.h"

FileMERData::FileMERData(std::vector<double> spectralData,
                         Core::Math::Vec2d spectralMinMax,
                         double spectralAverage):
_spectralMinMax(spectralMinMax),
_spectralData(spectralData),
_spectralAverage(spectralAverage),
_classification("?"){

}

FileMERData::~FileMERData(){

}

void FileMERData::setInput(std::vector<double> input){
    _rawData = input;
}


void FileMERData::setFrequency(double f){
    _frequency = f;
}

void FileMERData::setFrequencyRange(Core::Math::Vec2d range){
    _range = range;
}

void FileMERData::setDataPosition(Core::Math::Vec3f p){
    _position = p;
}

std::vector<double> FileMERData::getInput(){
    return _rawData;
}

std::vector<double> FileMERData::getSpectralPower(){
    return _spectralData;
}

Core::Math::Vec2d FileMERData::getFrequencyRange(){
    return _range;
}

Core::Math::Vec3f FileMERData::getDataPosition(){
    return _position;
}

Core::Math::Vec2d FileMERData::getMinMaxSpextralPower(){
    return _spectralMinMax;
}

double FileMERData::getSpectralAverage(){
    return _spectralAverage;
}

void FileMERData::executeFFT(){

}

std::string FileMERData::getClassification(){
    return _classification;
}
void FileMERData::setClassification(std::string classification){
    _classification = classification;
}

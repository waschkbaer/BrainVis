#include "FileMERData.h"

FileMERData::FileMERData(){

}

FileMERData::~FileMERData(){

}

void FileMERData::setInput(std::vector<double> input){
    _rawData = input;
}

void FileMERData::setSpectralPower(std::vector<double> power){
    _spectralData = power;
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

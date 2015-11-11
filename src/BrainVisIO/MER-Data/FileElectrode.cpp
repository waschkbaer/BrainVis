#include "FileElectrode.h"
#include <fstream>
#include <sstream>
#include <string>
#include <core/splittools.h>
#include <algorithm>

FileElectrode::FileElectrode(std::string name, std::string positionFile, std::string spectralFile):
    iElectrode(name){

}
FileElectrode::~FileElectrode(){


}

int8_t FileElectrode::addData(int8_t depth, iMERData* data){
    return 1;
}
iMERData* FileElectrode::getData(int8_t depth){
    return NULL;
}

bool FileElectrode::loadFiles(std::string Position, std::string Spectral){
    int curDepth = -10;


    std::ifstream filePosition(Position.c_str());
    if(!filePosition.is_open()){
        std::cout << "could not open file"<< std::endl;
        return false;
    }

    std::ifstream fileSpectral(Spectral.c_str());
    if(!fileSpectral.is_open()){
        std::cout << "could not open file"<< std::endl;
        return false;
    }

    std::string linePosition;
    std::string lineSpectral;
    while (std::getline(filePosition, linePosition) && std::getline(fileSpectral, lineSpectral))
    {
        Core::Math::Vec3f curPosition;
        std::vector<double> spectralPower;

        std::vector<std::string> valuesPosition = splitString(linePosition,',');
        curPosition = Core::Math::Vec3f(std::stof(valuesPosition[0]),std::stof(valuesPosition[1]),std::stof(valuesPosition[2]));

        std::vector<std::string> valuesSpectral = splitString(lineSpectral,',');
        for(int i = 0; i < valuesSpectral.size();++i){
            spectralPower.push_back(std::stod(valuesSpectral[i]));
        }

    }

    filePosition.close();
    fileSpectral.close();
}

#include "FileElectrode.h"
#include <fstream>
#include <sstream>
#include <string>
#include <core/splittools.h>
#include <algorithm>

#include "FileMERData.h"

FileElectrode::FileElectrode(std::string name,Core::Math::Vec2d range, std::string positionFile, std::string spectralFile):
    iElectrode(name,range){
    loadFiles(positionFile,spectralFile);
}
FileElectrode::~FileElectrode(){


}

bool FileElectrode::loadFiles(std::string Position, std::string Spectral){
    int curDepth = -10;
    float counter = 0;
    double max = -1000000.0;
    double min = 1000000.0;

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
        double powerAverage = 0.0f;
        max = -1000000.0;
        min = 1000000.0;
        int curFreq = 5;

        std::vector<std::string> valuesPosition = splitString(linePosition,',');
        curPosition = Core::Math::Vec3f(std::stof(valuesPosition[0]),std::stof(valuesPosition[1]),std::stof(valuesPosition[2]));

        std::vector<std::string> valuesSpectral = splitString(lineSpectral,',');
        for(int i = 0; i < valuesSpectral.size();++i){
            if(curFreq >= getAnalysisRange().x && curFreq <= getAnalysisRange().y){
                spectralPower.push_back(std::stod(valuesSpectral[i]));
                max = std::max(max,std::stod(valuesSpectral[i]));
                min = std::min(min,std::stod(valuesSpectral[i]));
                powerAverage += std::stod(valuesSpectral[i]);
            }
            curFreq+= 5;
        }
        powerAverage /= valuesSpectral.size();
        std::shared_ptr<iMERData> data = std::make_shared<FileMERData>(spectralPower,
                                                                       Core::Math::Vec2d(min,max),
                                                                       powerAverage);
        data->setDataPosition(curPosition);

        addData(curDepth,data);
        _SpectralAverage += powerAverage;
        counter++;
        curDepth++;
    }

    _SpectralAverage /= counter;

    filePosition.close();
    fileSpectral.close();
}

void FileElectrode::updateElectrode(){

}

void FileElectrode::recalculateFrequencyRange(){

}

#include "FileMERData.h"
#include <fstream>
#include <sstream>
#include <string>
#include <core/splittools.h>
#include <algorithm>

FileElectrode::FileElectrode(std::string name,std::string positions, std::string spectral, Core::Math::Vec2ui range){
    setName(name);
    this->setElektrodeRange(Core::Math::Vec2f(1000,-1000));
    loadPositions(positions);
    loadFFT(spectral,range);
}

FileElectrode::~FileElectrode(){

}

std::shared_ptr<MERData> FileElectrode::getData(int32_t depth){
    for(int i = 0; i < _data.size();++i){
        if(_data[i]->depth() == depth){
            return _data[i];
        }
    }
    return nullptr;
}

void FileElectrode::setData(int32_t depth,std::shared_ptr<MERData> data){

}

Core::Math::Vec2i FileElectrode::getDepthRange(){
    return Core::Math::Vec2i(-10,4);
}

bool FileElectrode::loadPositions(std::string positions){
    int curDepth = -10;
    Core::Math::Vec3f curPosition;

    std::ifstream file(positions.c_str());
    if(!file.is_open()){
        std::cout << "could not open file"<< std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> values = splitString(line,',');
        curPosition = Core::Math::Vec3f(std::stof(values[0]),std::stof(values[1]),std::stof(values[2]));

        std::shared_ptr<MERData> data = std::make_shared<MERData>(curDepth,curPosition,0,std::vector<float>(),std::vector<float>());
        curDepth++;

        _data.push_back(data);
    }

    file.close();

    return true;

}

bool FileElectrode::loadFFT(std::string spectral, Core::Math::Vec2ui range){
    int curDepth = -10;
    float avg = 0;
    Core::Math::Vec3f curPosition;

    std::ifstream file(spectral.c_str());
    if(!file.is_open()){
        std::cout << "could not open file"<< std::endl;
        return false;
    }

    std::string line;
    int freqcounter = 0;
    int avgcounter = 0;
    float min;
    float max;
    while (std::getline(file, line))
    {
        std::vector<std::string> values = splitString(line,',');
        avg = 0;
        avgcounter =0;
        std::vector<float> spectralvalues;
        freqcounter = 0;
        min = 1000.0f;
        max = -1000.0f;

        float MAXSENSEVALUE = 10.0f;

        for(int i = 0; i < values.size();++i){
            if(freqcounter >= range.x && freqcounter <= range.y){
                spectralvalues.push_back(std::stof(values[i]));
                avg += std::min(MAXSENSEVALUE,std::stof(values[i]));
                avgcounter++;
                if(min > std::stof(values[i])){
                    min = std::stof(values[i]);
                }
                if(max < std::stof(values[i])){
                    max = std::stof(values[i]);
                }
            }
            freqcounter+=5;
        }
        avg /= avgcounter;
        min = std::max(0.0f,min);
        max = std::min(MAXSENSEVALUE,max);

        std::shared_ptr<MERData> data = getData(curDepth);
        data->setFreqRange(Core::Math::Vec2ui(range.x,range.y));
        data->setSpectralFlow(spectralvalues);

        Core::Math::Vec2f range(min,max);
        data->setSpectralRange(range);

        Core::Math::Vec2f curRange = this->getElektrodeRange();
        curRange.x = std::min(curRange.x,min);
        curRange.y = std::max(curRange.y,max);
        this->setElektrodeRange(curRange);

        data->setSpectralPower(avg);
        std::cout << spectral << "   - avg "<< avg <<" rangElek "<< curRange << std::endl;
        curDepth++;
    }

    file.close();


    return true;
}



FileElectrodeBundle::FileElectrodeBundle(
        std::string name1, std::string positions1, std::string spectral1,
        std::string name2, std::string positions2, std::string spectral2,
        std::string name3, std::string positions3, std::string spectral3
){
    std::shared_ptr<FileElectrode> f1 = std::make_shared<FileElectrode>(name1,positions1,spectral1,Core::Math::Vec2ui(0,2000));
    std::shared_ptr<FileElectrode> f2 = std::make_shared<FileElectrode>(name2,positions2,spectral2,Core::Math::Vec2ui(0,2000));
    std::shared_ptr<FileElectrode> f3 = std::make_shared<FileElectrode>(name3,positions3,spectral3,Core::Math::Vec2ui(0,2000));
    _trajectories.push_back(f1);
    _trajectories.push_back(f2);
    _trajectories.push_back(f3);
}

FileElectrodeBundle::FileElectrodeBundle(
        std::vector<std::string> name,
        std::vector<std::string> position,
        std::vector<std::string> spectral,
        Core::Math::Vec2ui range
        ){
    for(int i = 0; i < name.size();++i){
        _trajectories.push_back(std::make_shared<FileElectrode>(name[i],position[i],spectral[i],range));
    }
}


FileElectrodeBundle::~FileElectrodeBundle(){

}

std::shared_ptr<AbstrElectrode> FileElectrodeBundle::getTrajectory(uint32_t i){
    return _trajectories[i];
}

uint32_t FileElectrodeBundle::getTrajectoryCount(){
    return _trajectories.size();
}

void FileElectrodeBundle::addTrajectory(std::shared_ptr<AbstrElectrode> traj){
    _trajectories.push_back(traj);
}


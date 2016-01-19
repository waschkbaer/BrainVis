#include "MERElectrode.h"
#include "MERData.h"

#include <iostream>

using namespace BrainVisIO::MERData;

MERElectrode::MERElectrode(Core::Math::Vec3f startPosition,
                           int startDepth):
    _currentDepth(startDepth),
    _startPosition(startPosition)
{
    _data.insert(std::pair<int,std::shared_ptr<MERData>>
                        (
                            _currentDepth,
                            std::make_shared<MERData>(_currentDepth))
                        );
}

MERElectrode::MERElectrode(const std::vector<std::string>& filelist,
                           Core::Math::Vec3f startPosition,
                           int startDepth):
    _currentDepth(startDepth),
    _startPosition(startPosition)
{
    loadRecordings(filelist);
}


MERElectrode::~MERElectrode(){

}

void MERElectrode::newRecording(){
    _currentDepth++;
    _data.insert(std::pair<int,std::shared_ptr<MERData>>
                        (
                            _currentDepth,
                            std::make_shared<MERData>(_currentDepth))
                        );
}

void MERElectrode::calculateFFT(){
    std::map<int,std::shared_ptr<MERData>>::iterator it = _data.begin();
    for(it = _data.begin(); it != _data.end(); ++it){
        it->second->executeFFTWelch(5,true);
    }
}

int MERElectrode::getLatestDepth(){
    return _currentDepth;
}

const std::shared_ptr<MERData> MERElectrode::getMERData(int depth){
    if(_data.find(depth) != _data.end()){
        return _data.find(depth)->second;
    }
    return nullptr;
}

void MERElectrode::loadRecordings(const std::vector<std::string>& filelist){
    for(const std::string s : filelist){
        _data.insert(std::pair<int,std::shared_ptr<MERData>>
                            (
                                _currentDepth,
                                std::make_shared<MERData>(_currentDepth,s))
                            );
        _currentDepth++;
    }
    _currentDepth--;
}

#include "MERElectrode.h"
#include "MERData.h"

#include <iostream>

using namespace BrainVisIO::MERData;

MERElectrode::MERElectrode(Core::Math::Vec3f targetPosition,
                           int startDepth):
    _currentDepth(startDepth),
    _targetPosition(targetPosition),
    _elctrodeDirection(Core::Math::Vec3f(0,1,0)),
    _electrodeVisible(true)
{
    _data.insert(std::pair<int,std::shared_ptr<MERData>>
                        (
                            _currentDepth,
                            std::make_shared<MERData>(_currentDepth))
                        );
}

MERElectrode::MERElectrode(const std::vector<std::string>& filelist,
                           Core::Math::Vec3f targetPosition,
                           int startDepth):
    _currentDepth(startDepth),
    _targetPosition(targetPosition),
    _elctrodeDirection(Core::Math::Vec3f(0,1,0)),
    _electrodeVisible(true)
{
    loadRecordings(filelist);
}


MERElectrode::~MERElectrode(){

}

void MERElectrode::newRecording(){
    _currentDepth++;

    std::shared_ptr<MERData> data = std::make_shared<MERData>(_currentDepth);
    data->setPosition(_targetPosition + _elctrodeDirection*_currentDepth);

    _data.insert(std::pair<int,std::shared_ptr<MERData>>
                        (
                            _currentDepth,
                            data)
                        );
}

void MERElectrode::newRecording(int depth){
    //std::cout << _currentDepth << " --- "<< depth << std::endl;
    //if(depth <= _currentDepth) return;
    _currentDepth = depth;

    std::shared_ptr<MERData> data = std::make_shared<MERData>(_currentDepth);
    data->setPosition(_targetPosition + _elctrodeDirection*_currentDepth);

    _data.insert(std::pair<int,std::shared_ptr<MERData>>
                        (
                            _currentDepth,
                            data)
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

void MERElectrode::deselectAllData(){
    std::map<int,std::shared_ptr<MERData>>::iterator it = _data.begin();
    for(it = _data.begin(); it != _data.end(); ++it){
        it->second->setFocusSelected(false);
    }
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
bool MERElectrode::getElectrodeVisible() const
{
    return _electrodeVisible;
}

void MERElectrode::setElectrodeVisible(bool electrodeVisible)
{
    _electrodeVisible = electrodeVisible;
}

Core::Math::Vec3f MERElectrode::getElctrodeDirection() const
{
    return _elctrodeDirection;
}

void MERElectrode::setElctrodeDirection(const Core::Math::Vec3f &elctrodeDirection)
{
    _elctrodeDirection = elctrodeDirection;
}

Core::Math::Vec3f MERElectrode::getTargetPosition() const
{
    return _targetPosition;
}

void MERElectrode::setTargetPosition(const Core::Math::Vec3f &targetPosition)
{
    _targetPosition = targetPosition;
}


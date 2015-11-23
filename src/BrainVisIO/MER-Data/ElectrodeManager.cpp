#include "ElectrodeManager.h"

ElectrodeManager::ElectrodeManager():
_electrodes(),
_trackedElectrode("none"){
}

ElectrodeManager::~ElectrodeManager(){

}
std::string ElectrodeManager::getTrackedElectrode() const
{
    return _trackedElectrode;
}

void ElectrodeManager::setTrackedElectrode(const std::string &trackedElectrode)
{
    _trackedElectrode = trackedElectrode;
}


std::shared_ptr<iElectrode> ElectrodeManager::getElectrode(std::string name){
    if(_electrodes.find(name) != _electrodes.end()){
        return _electrodes.find(name)->second;
    }
    return nullptr;
}

std::shared_ptr<iElectrode> ElectrodeManager::getElectrode(int i){
    return getElectrode(_names[i]);
}

int ElectrodeManager::getElectrodeCount() const{
    return _electrodes.size();
}

void ElectrodeManager::addElectrode(std::shared_ptr<iElectrode> electrode){
    if(_electrodes.find(electrode->getName()) == _electrodes.end()){
        _electrodes.insert(std::pair<std::string,std::shared_ptr<iElectrode>>(electrode->getName(),electrode));
        _names.push_back(electrode->getName());
    }
}

void ElectrodeManager::updateElectrodes(){
    for(int i = 0; i < _electrodes.size();++i){
        getElectrode(i)->updateElectrode();
    }
}

std::vector<std::string> ElectrodeManager::getRegisteredElectrodes() const{
    return _names;
}

void ElectrodeManager::clear(){
    _electrodes.clear();
    _names.clear();
}

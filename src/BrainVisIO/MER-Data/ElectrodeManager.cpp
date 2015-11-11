#include "ElectrodeManager.h"

ElectrodeManager::ElectrodeManager():
_electrodes(){
std::cout << "created electrode manager --------------------"<<std::endl;
}

ElectrodeManager::~ElectrodeManager(){

}

std::shared_ptr<iElectrode> ElectrodeManager::getElectrode(std::string name){
    if(_electrodes.find(name) != _electrodes.end()){
        std::cout << name.c_str() << "  " << _electrodes.find(name)->second->getName().c_str() <<  std::endl;
        return _electrodes.find(name)->second;
    }
    return nullptr;
}

std::shared_ptr<iElectrode> ElectrodeManager::getElectrode(int i){
    std::string name = _names[i];
    return getElectrode(name);
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

#include "MERBundle.h"
#include "MERElectrode.h"

using namespace BrainVisIO::MERData;

MERBundle::MERBundle(){

}
MERBundle::MERBundle(const std::vector<std::string>& electrodeList){
    for(const std::string s : electrodeList){
        addElectrode(s);
    }
}
MERBundle::~MERBundle(){

}

void MERBundle::addElectrode(const std::string& name, std::shared_ptr<MERElectrode> electrode){
    if(electrode == nullptr){
        _electrodes.insert(std::pair<std::string,std::shared_ptr<MERElectrode>>
                           (
                               name,
                               std::make_shared<MERElectrode>())
                           );
    }else{
        _electrodes.insert(std::pair<std::string,std::shared_ptr<MERElectrode>>
                           (
                               name,
                               electrode)
                           );
    }
}

const std::shared_ptr<MERElectrode> MERBundle::getElectrode(const std::string& name){
    if(_electrodes.find(name) == _electrodes.end()){
        return nullptr;
    }else{
        return _electrodes.find(name)->second;
    }
}

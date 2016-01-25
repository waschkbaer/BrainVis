#include "MERBundle.h"
#include "MERElectrode.h"
#include "MERData.h"

using namespace BrainVisIO::MERData;

MERBundle::MERBundle():
    _isRightSide(false),
    _target(0,0,0),
    _entry(0,0,0),
    _isActive(false){

}
MERBundle::MERBundle(const std::vector<std::string>& electrodeList):
    _isRightSide(false),
    _target(0,0,0),
    _entry(0,0,0),
    _isActive(false){
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

void  MERBundle::calculateElectrodePosition(Core::Math::Vec3f XAxis, Core::Math::Vec3f YAxis,
                                            Core::Math::Vec3f target, Core::Math::Vec3f entry){

    std::shared_ptr<MERElectrode> electrode = _electrodes.find("cen")->second;
    Core::Math::Vec3f direction = target-entry;
    direction.normalize();

    electrode->setTargetPosition(target); //center is directly in target, no calculation needed
    electrode->setElctrodeDirection(direction);

    calculateDataPosition(electrode);

    //anterior (vorne)
    Core::Math::Vec3f anDir = XAxis % direction;
    anDir.normalize();
    anDir = target + -2.0f*anDir;
    electrode = _electrodes.find("ant")->second;

    electrode->setTargetPosition(anDir);
    electrode->setElctrodeDirection(direction);

    calculateDataPosition(electrode);

    //lateral (seite)
    Core::Math::Vec3f laDir = YAxis % direction;
    laDir.normalize();
    laDir = target + -2.0f*laDir;
    electrode = _electrodes.find("lat")->second;

    electrode->setTargetPosition(laDir);
    electrode->setElctrodeDirection(direction);

    calculateDataPosition(electrode);
}

void MERBundle::calculateDataPosition(std::shared_ptr<MERElectrode> electrode){
    std::shared_ptr<MERData> data = nullptr;
    Core::Math::Vec3f position;
    for(float i = -10; i <= 5;++i){
        data = electrode->getMERData(i);
        if(data != nullptr){
            position = electrode->getTargetPosition() + (i*electrode->getElctrodeDirection());
            data->setPosition(position);
        }
    }
}
bool MERBundle::getIsRightSide() const
{
    return _isRightSide;
}

void MERBundle::setIsRightSide(bool isRightSide)
{
    _isRightSide = isRightSide;
}

Core::Math::Vec3f MERBundle::getEntry() const
{
    return _entry;
}

void MERBundle::setEntry(const Core::Math::Vec3f &entry)
{
    _entry = entry;
}

Core::Math::Vec3f MERBundle::getTarget() const
{
    return _target;
}

void MERBundle::setTarget(const Core::Math::Vec3f &target)
{
    _target = target;
}

bool MERBundle::getIsActive() const
{
    return _isActive;
}

void MERBundle::setIsActive(bool isActive)
{
    _isActive = isActive;
}


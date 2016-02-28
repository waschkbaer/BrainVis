#include "MERBundleManager.h"

#include "MERBundle.h"

using namespace BrainVisIO::MERData;

void MERBundleManager::addMERBundle(const std::string& identifier, const std::shared_ptr<MERBundle> bundle){
    if(_bundles.find(identifier) == _bundles.end())
        _bundles.insert(std::pair<std::string,std::shared_ptr<MERBundle>>(identifier,bundle));
    else
       _bundles[identifier] = bundle;
}

void MERBundleManager::removeMERBundle(const std::string& identifier){
    _bundles.erase(identifier);
}

const std::shared_ptr<MERBundle> MERBundleManager::getMERBundle(const std::string& identifier){
    if(_bundles.find(identifier) == _bundles.end()){
        return nullptr;
    }else{
        return _bundles.find(identifier)->second;
    }
}

const std::vector<std::string> MERBundleManager::getRegisteredBundles(){
    std::map<std::string,std::shared_ptr<MERBundle>>::iterator it = _bundles.begin();
    std::vector<std::string> registeredBundles;
    for(it = _bundles.begin(); it != _bundles.end(); ++it){
        registeredBundles.push_back(it->first);
    }
    return registeredBundles;
}

void MERBundleManager::disableAllBundles(){
    std::map<std::string,std::shared_ptr<MERBundle>>::iterator it = _bundles.begin();
    for(it = _bundles.begin(); it != _bundles.end(); ++it){
        it->second->setIsActive(false);
    }
}

void MERBundleManager::deselectAllData(){
    std::map<std::string,std::shared_ptr<MERBundle>>::iterator it = _bundles.begin();
    for(it = _bundles.begin(); it != _bundles.end(); ++it){
        it->second->deselectAllData();
    }
}

void MERBundleManager::activateBundle(const std::string& name){
    disableAllBundles();
    getMERBundle(name)->setIsActive(true);
    _activeBundleName = name;
}
std::string MERBundleManager::getActiveBundleName() const
{
    return _activeBundleName;
}
uint64_t MERBundleManager::getBundleStatus() const
{
    return _bundleStatus;
}

void MERBundleManager::incrementBundleStatus(){
    _bundleStatus++;
}


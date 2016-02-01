#include "DICOMRenderManager.h"


uint16_t DicomRenderManager::addRenderer(std::shared_ptr<DICOMRenderer> r){
    uint16_t id = _nextRenderID;
    if(r == nullptr){
        _rendererMap.insert(std::pair<uint16_t,std::shared_ptr<DICOMRenderer>>(id, std::make_shared<DICOMRenderer>()));
    }else{
        _rendererMap.insert(std::pair<uint16_t,std::shared_ptr<DICOMRenderer>>(id, r));
    }
    _nextRenderID++;
    return id;
}

const std::shared_ptr<DICOMRenderer> DicomRenderManager::getRenderer(RenderMode mode){
    std::map<uint16_t,std::shared_ptr<DICOMRenderer>>::iterator it = _rendererMap.begin();
    std::shared_ptr<DICOMRenderer> r = nullptr;
    for(it = _rendererMap.begin(); it != _rendererMap.end(); ++it){
        if(mode == it->second->activeRenderMode()){
            r = it->second;
            it = _rendererMap.end();
            break;
        }
    }
    return r;
}

void DicomRenderManager::setTrackMode(bool mode){
    std::map<uint16_t,std::shared_ptr<DICOMRenderer>>::iterator it = _rendererMap.begin();
    std::shared_ptr<DICOMRenderer> r = nullptr;
    for(it = _rendererMap.begin(); it != _rendererMap.end(); ++it){
            r = it->second;
            r->setIsTracking(mode);
            std::cout << mode << std::endl;

    }
}

const std::shared_ptr<DICOMRenderer> DicomRenderManager::getRenderer(uint16_t id){
    if(_rendererMap.find(id) != _rendererMap.end()){
        return _rendererMap.find(id)->second;
    }
    return nullptr;
}

void DicomRenderManager::deleteRenderer(uint16_t id){
    if(_rendererMap.find(id) != _rendererMap.end()){
        std::shared_ptr<DICOMRenderer> d = _rendererMap.find(id)->second;
        d.reset();
        _rendererMap.erase(id);
    }
}

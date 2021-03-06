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

void DicomRenderManager::recreateRenderer(int id){
    if(_rendererMap.find(id) != _rendererMap.end()){
       _rendererMap.find(id)->second = std::make_shared<DICOMRenderer>();
    }
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

void DicomRenderManager::forceRepaint(){
    std::map<uint16_t,std::shared_ptr<DICOMRenderer>>::iterator it = _rendererMap.begin();
    std::shared_ptr<DICOMRenderer> r = nullptr;
    for(it = _rendererMap.begin(); it != _rendererMap.end(); ++it){
            r = it->second;
            r->sheduleRepaint();

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
float DicomRenderManager::getBlendValue() const
{
    return _blendValue;
}

void DicomRenderManager::setBlendValue(float blendValue)
{
    _blendValue = blendValue;
}
float DicomRenderManager::getPerformanceValue() const
{
    return _performanceValue;
}

void DicomRenderManager::setPerformanceValue(float performanceValue)
{
    _performanceValue = performanceValue;
}
bool DicomRenderManager::getDisplayFrameShapes() const
{
    return _displayFrameShapes;
}

void DicomRenderManager::setDisplayFrameShapes(bool displayFrameShapes)
{
    _displayFrameShapes = displayFrameShapes;
}
bool DicomRenderManager::getDisplayFrameDetectionBox() const
{
    return _displayFrameDetectionBox;
}

void DicomRenderManager::setDisplayFrameDetectionBox(bool displayFrameDetectionBox)
{
    _displayFrameDetectionBox = displayFrameDetectionBox;
    _renderSettingStatus++;
}
bool DicomRenderManager::getDisplayBoundingBox() const
{
    return _displayBoundingBox;
}

void DicomRenderManager::setDisplayBoundingBox(bool displayBoundingBox)
{
    _displayBoundingBox = displayBoundingBox;
}
bool DicomRenderManager::getDisplayElectrodes() const
{
    return _displayElectrodes;
}

void DicomRenderManager::setDisplayElectrodes(bool displayElectrodes)
{
    _displayElectrodes = displayElectrodes;
}
bool DicomRenderManager::getOrthonormalThreeD() const
{
    return _orthonormalThreeD;
}

void DicomRenderManager::setOrthonormalThreeD(bool orthonormalThreeD)
{
    _orthonormalThreeD = orthonormalThreeD;
}
int DicomRenderManager::getBlendoption() const
{
    return _blendoption;
}

void DicomRenderManager::setBlendoption(int blendoption)
{
    _blendoption = blendoption;
}

std::shared_ptr<std::vector<Core::Math::Vec4f>> DicomRenderManager::getTransferFunction()
{
    return _transferFunction->GetColorData();
}

void DicomRenderManager::setSmoothStep(float pos, float grad){
    _position = pos;
    _gradient = grad;

    _position = std::min(1.0f,std::max(0.0f,_position));
    _gradient = std::min(1.0f,std::max(0.0f,_gradient));

    _transferFunction->SetStdFunction(_position,_gradient);

    _renderSettingStatus++;
}

float DicomRenderManager::getGradient() const
{
    return _gradient;
}
uint64_t DicomRenderManager::getRenderSettingStatus() const
{
    return _renderSettingStatus;
}


float DicomRenderManager::getPosition() const
{
    return _position;
}



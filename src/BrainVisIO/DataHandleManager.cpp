#include "DataHandleManager.h"


uint16_t DataHandleManager::createNewDataHandle(){
    _highestDHid++;
    _datahandles.insert( std::pair<uint16_t,std::shared_ptr<DataHandle>>(_highestDHid, std::make_shared<DataHandle>()));
    return _highestDHid;
}

const std::shared_ptr<DataHandle> DataHandleManager::getDataHandle(uint16_t id){
    if(_datahandles.find(id) != _datahandles.end()){
        return _datahandles.find(id)->second;
    }
    return nullptr;
}

void DataHandleManager::deleteDataHandle(uint16_t id){
    if(_datahandles.find(id) != _datahandles.end()){
        std::shared_ptr<DataHandle> d = _datahandles.find(id)->second;
        d.reset();
        _datahandles.erase(id);
    }
}

#ifndef DATAHANDLERMANAGER_H
#define DATAHANDLERMANAGER_H

#include <map>
#include <memory>

#include "DataHandle.h"

class DataHandleManager{
public:
    static DataHandleManager& getInstance(){
        static DataHandleManager instance;
        return instance;
    }

    uint16_t createNewDataHandle();
    const std::shared_ptr<DataHandle> getDataHandle(uint16_t id);
    void deleteDataHandle(uint16_t id);

private:
    uint16_t                                        _highestDHid;
    std::map<uint16_t, std::shared_ptr<DataHandle>> _datahandles;

    DataHandleManager():_highestDHid(0){};
    ~DataHandleManager(){};
};

#endif // DATAHANDLER_H

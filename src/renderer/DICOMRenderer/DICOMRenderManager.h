#ifndef __TNG__DICOMRENDERMANAGER__
#define __TNG__DICOMRENDERMANAGER__

#include <map>
#include <string>
#include <memory>

#include "DICOMRenderer.h"

class DicomRenderManager{
public:
    static DicomRenderManager& getInstance(){
        static DicomRenderManager instance;
        return instance;
    }

    uint16_t addRenderer(std::shared_ptr<DICOMRenderer> r = nullptr);
    const std::shared_ptr<DICOMRenderer> getRenderer(uint16_t);
    void deleteRenderer(uint16_t id);

protected:

private:
    std::map<uint16_t,std::shared_ptr<DICOMRenderer>> _rendererMap;
    uint16_t                                          _nextRenderID;

    DicomRenderManager():
    _nextRenderID(0){}
    ~DicomRenderManager(){}
};

#endif

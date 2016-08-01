#ifndef __TNG__DICOMRENDERMANAGER__
#define __TNG__DICOMRENDERMANAGER__

#include <map>
#include <string>
#include <memory>

#include "DICOMRenderer.h"
#include "DICOMRendererEnums.h"
#include <io/TransferFunction1D.h>

class DicomRenderManager{
public:
    static DicomRenderManager& getInstance(){
        static DicomRenderManager instance;
        return instance;
    }

    uint16_t addRenderer(std::shared_ptr<DICOMRenderer> r = nullptr);
    void recreateRenderer(int id);
    const std::shared_ptr<DICOMRenderer> getRenderer(uint16_t);
    const std::shared_ptr<DICOMRenderer> getRenderer(RenderMode mode);
    void setTrackMode(bool mode);
    void deleteRenderer(uint16_t id);

    void forceRepaint();

    float getBlendValue() const;
    void setBlendValue(float blendValue);

    float getPerformanceValue() const;
    void setPerformanceValue(float performanceValue);

    bool getDisplayFrameShapes() const;
    void setDisplayFrameShapes(bool displayFrameShapes);

    bool getDisplayFrameDetectionBox() const;
    void setDisplayFrameDetectionBox(bool displayFrameDetectionBox);

    bool getDisplayBoundingBox() const;
    void setDisplayBoundingBox(bool displayBoundingBox);

    bool getDisplayElectrodes() const;
    void setDisplayElectrodes(bool displayElectrodes);

    bool getOrthonormalThreeD() const;
    void setOrthonormalThreeD(bool orthonormalThreeD);

    int getBlendoption() const;
    void setBlendoption(int blendoption);

    std::shared_ptr<std::vector<Core::Math::Vec4f>> getTransferFunction();
    void setSmoothStep(float pos, float grad);

    float getPosition() const;
    float getGradient() const;
    uint64_t getRenderSettingStatus() const;

protected:

private:
    std::map<uint16_t,std::shared_ptr<DICOMRenderer>> _rendererMap;
    uint16_t                                          _nextRenderID;

private:
    //global rendererstates
    float           _blendValue;
    float           _performanceValue;
    bool            _displayFrameShapes;
    bool            _displayFrameDetectionBox;
    bool            _displayBoundingBox;
    bool            _displayElectrodes;
    bool            _orthonormalThreeD;
    int             _blendoption;

    //Transferfunction
    std::unique_ptr<DataIO::TransferFunction1D>     _transferFunction;
    float                                           _position;
    float                                           _gradient;
    uint64_t        _renderSettingStatus;

    DicomRenderManager():
    _nextRenderID(0),
    _blendValue(0.5f),
    _performanceValue(0.001f),
    _displayFrameShapes(true),
    _displayFrameDetectionBox(false),
    _displayBoundingBox(true),
    _displayElectrodes(true),
    _orthonormalThreeD(false),
    _blendoption(0),
    _transferFunction(nullptr),
      _renderSettingStatus(0)
    {
        _transferFunction = std::unique_ptr<DataIO::TransferFunction1D>(new DataIO::TransferFunction1D(4000));
        _transferFunction->SetStdFunction();
    }
    ~DicomRenderManager(){}
};

#endif

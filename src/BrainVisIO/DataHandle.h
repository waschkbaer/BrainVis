#ifndef DATAHANDLER_H
#define DATAHANDLER_H


#include <core/FileFinder.h>


#include <BrainVisIO/DicomVolume.h>
#include <core/Math/Vectors.h>
#include <BrainVisIO/TransferFunction1D.h>
#include <BrainVisIO/MER-Data/ElectrodeManager.h>

#include <BrainVisIO/MER-Data/MERConnection.h>

#include <memory>
#include <thread>
#include <vector>

struct Trajectory{
    Trajectory():
        _startPlaning(0,0,0),
        _endPlaning(0,0,0),
        _startWorldSpace(0,0,0),
        _endWorldSpace(0,0,0),
        _startVolumeSpace(0,0,0),
        _endVolumeSpace(0,0,0)
    {}

    Trajectory(Core::Math::Vec3f startP, Core::Math::Vec3f endP):
        _startPlaning(startP),
        _endPlaning(endP),
        _startWorldSpace(0,0,0),
        _endWorldSpace(0,0,0),
        _startVolumeSpace(0,0,0),
        _endVolumeSpace(0,0,0)
    {}

    Core::Math::Vec3f _startPlaning;
    Core::Math::Vec3f _endPlaning;
    Core::Math::Vec3f _startWorldSpace;
    Core::Math::Vec3f _endWorldSpace;
    Core::Math::Vec3f _startVolumeSpace;
    Core::Math::Vec3f _endVolumeSpace;
};

class DataHandle
{
public:
    DataHandle();
    virtual ~DataHandle(){};

    void loadMRData(std::string& path);
    void loadCTData(std::string& path);

    Core::Math::Vec3f getSelectedSlices() const;
    Core::Math::Vec3ui getMRDimensions() const { return _MRVolume->getDimensions();}
    Core::Math::Vec3ui getCTDimensions() const { return _CTVolume->getDimensions();}
    void setSelectedSlices(Core::Math::Vec3f slides);

    float getMRTransferScaling() const;
    float getCTTransferScaling() const;

    std::shared_ptr<std::vector<Core::Math::Vec4f>> getTransferFunction();
    void setSmoothStep(float pos, float grad);

    std::vector<char>&      getMRData() {return _MRVolume->getData(); }
    std::vector<char>&      getCTData() {return _CTVolume->getData(); }
    Core::Math::Vec3f       getMRAspectRatio() {return _MRVolume->getAspectRatio();}
    Core::Math::Vec3f       getCTAspectRatio() {return _CTVolume->getAspectRatio();}

    //***************
    std::string getMRPath() const;
    void setMRPath(const std::string &MRPath);

    Core::Math::Vec3f getMROffset() const;
    void setMROffset(const Core::Math::Vec3f &MROffset);

    Core::Math::Vec3f getMRScale() const;
    void setMRScale(const Core::Math::Vec3f &MRScale);


    float getFMRScalingFactor() const;
    void setFMRScalingFactor(float fMRScalingFactor);

    //CT
    std::string getCTPath() const;
    void setCTPath(const std::string &CTPath);

    Core::Math::Vec3f getCTScale() const;
    void setCTScale(const Core::Math::Vec3f &CTScale);

    float getFCTScalingFactor() const;
    void setFCTScalingFactor(float fCTScalingFactor);

    bool getBFoundCTFrame() const;
    void setBFoundCTFrame(bool bFoundCTFrame);

    std::vector<Core::Math::Vec3f> getLeftMarker() const;
    void setLeftMarker(const std::vector<Core::Math::Vec3f> &leftMarker);

    std::vector<Core::Math::Vec3f> getRightMarker() const;
    void setRightMarker(const std::vector<Core::Math::Vec3f> &rightMarker);

    Core::Math::Mat4f getCTUnitMatrix() const;

    Core::Math::Vec3f getCTCenter() const;
    void setCTCenter(const Core::Math::Vec3f &CTCenter);

    //Planning
    Core::Math::Vec3f getAC() const;
    void setAC(const Core::Math::Vec3f &vAC);

    Core::Math::Vec3f getPC() const;
    void setPC(const Core::Math::Vec3f &vPC);

    Core::Math::Vec3f getMR() const;
    void setMR(const Core::Math::Vec3f &vMR);

    Core::Math::Vec3f getCTeX() const;
    void setCTeX(const Core::Math::Vec3f &CTeX);

    Core::Math::Vec3f getCTeY() const;
    void setCTeY(const Core::Math::Vec3f &CTeY);

    Core::Math::Vec3f getCTeZ() const;
    void setCTeZ(const Core::Math::Vec3f &CTeZ);

    Trajectory getLeftSTN() const;
    void setLeftSTN(const Trajectory &leftSTN);

    Trajectory getRightSTN() const;
    void setRightSTN(const Trajectory &rightSTN);

    Core::Math::Mat4f getMRWorld() const;
    void setMRWorld(const Core::Math::Mat4f &MRWorld);

    Core::Math::Mat4f getCTWorld() const;
    void setCTWorld(const Core::Math::Mat4f &CTWorld);

    bool calculateCTUnitVectors();

    Core::Math::Vec3f getCTOffset() const;
    void setCTOffset(const Core::Math::Vec3f &CTOffset);

    bool getMRLoaded() const;
    void setMRLoaded(bool MRLoaded);

    bool getCTLoaded() const;
    void setCTLoaded(bool CTLoaded);

    Core::Math::Vec3f getMRRotation() const;
    void setMRRotation(const Core::Math::Vec3f &MRRotation);

    float getMRCTBlend() const;
    void setMRCTBlend(float MRCTBlend);

    Core::Math::Vec2f getSpectralRange() const;

    uint64_t getDataSetStatus() const;

    Core::Math::Vec2i getDisplayedDriveRange() const;
    void setDisplayedDriveRange(const Core::Math::Vec2i &displayedDriveRange);

    std::vector<Core::Math::Vec3f> getFFTColorImage() const;
    void createFFTColorImage();

    void loadMERFiles(std::string& path,std::vector<std::string> types);
    void loadMERNetwork(std::vector<std::string> types);

    std::shared_ptr<iElectrode> getElectrode(std::string name);
    std::shared_ptr<iElectrode> getElectrode(int i);

    bool getUsesNetworkMER() const;
    void NetworkUpdateThread();
    void waitForNetworkThread();

    Core::Math::Vec3f getSelectedCTSpacePosition() const;

    Core::Math::Vec3f getSelectedWorldSpacePositon() const;

    void checkFocusPoint();
    void incrementStatus();
private:
    void updateMRWorld();


    //MR Volume
    std::string                                     _MRPath;
    std::unique_ptr<DicomVolume>                    _MRVolume;
    bool                                            _MRLoaded;
    Core::Math::Vec3f                               _MROffset;
    Core::Math::Vec3f                               _MRScale;
    Core::Math::Vec3f                               _MRRotation;
    Core::Math::Mat4f                               _MRWorld;
    float                                           _fMRScalingFactor;

    //CT Volume
    std::string                                     _CTPath;
    std::unique_ptr<DicomVolume>                    _CTVolume;
    bool                                            _CTLoaded;
    Core::Math::Vec3f                               _CTOffset;
    Core::Math::Vec3f                               _CTScale;
    Core::Math::Mat4f                               _CTWorld;
    float                                           _fCTScalingFactor;
    bool                                            _bFoundCTFrame;
    std::vector<Core::Math::Vec3f>                  _leftMarker;
    std::vector<Core::Math::Vec3f>                  _rightMarker;
    Core::Math::Mat4f                               _CTUnitMatrix;
    Core::Math::Vec3f                               _CTCenter;
    Core::Math::Vec3f                               _CTeX;
    Core::Math::Vec3f                               _CTeY;
    Core::Math::Vec3f                               _CTeZ;

    //Planing
    Core::Math::Vec3f                               _vAC;
    Core::Math::Vec3f                               _vPC;
    Core::Math::Vec3f                               _vMR;

    Core::Math::Vec2i                               _displayedDriveRange;
    Core::Math::Vec2f                               _spectralRange;
    Trajectory                                      _leftSTN;
    Trajectory                                      _rightSTN;
    float                                           _MRCTBlend;
    std::vector<Core::Math::Vec3f>                  _FFTColorImage;

    //Transferfunction
    std::unique_ptr<DataIO::TransferFunction1D>     _transferFunction;
    bool                                            _tfChanged;
    Core::Math::Vec3f                               _vSelectedVolumeSpacePosition;
    Core::Math::Vec3f                               _vSelectedCTSpacePosition;
    Core::Math::Vec3f                               _vSelectedWorldSpacePositon;

    uint64_t                                        _dataSetStatus;

    bool                                            _usesNetworkMER;
    std::unique_ptr<std::thread>                    _networkUpdateThread;

};

#endif // DATAHANDLER_H

#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <io/DicomVolume.h>


#include <core/Math/Vectors.h>
#include <io/TransferFunction1D.h>

#include <memory>
#include <thread>
#include <vector>

class DataHandle
{
public:
    DataHandle();
    virtual ~DataHandle(){};

    void loadMRData(const std::string& path);
    void loadCTData(const std::string& path);

    Core::Math::Vec3f getSelectedSlices() const;
    Core::Math::Vec3ui getMRDimensions() const;
    Core::Math::Vec3ui getCTDimensions() const;
    void setSelectedSlices(const Core::Math::Vec3f slides);

    float getMRTransferScaling() const;
    float getCTTransferScaling() const;

    const std::vector<uint16_t>&      getMRData();
    const std::vector<uint16_t>&      getCTData();
    Core::Math::Vec3f       getMRAspectRatio();
    Core::Math::Vec3f       getCTAspectRatio();

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

    void loadMERFiles(const std::string& path,const std::vector<std::string>& types);
    void loadMERNetwork(std::vector<std::string> types);

    bool getUsesNetworkMER() const;
    void NetworkUpdateThread();
    void waitForNetworkThread();

    Core::Math::Vec3f getSelectedCTSpacePosition() const;

    Core::Math::Vec3f getSelectedWorldSpacePositon() const;

    void setFocusPoint(Core::Math::Vec3f position);
    void incrementStatus();

    float getFTranslationStep() const;
    void setFTranslationStep(float fTranslationStep);

    float getFTranslationStepScale() const;
    void setFTranslationStepScale(float fTranslationStepScale);

    float getFRotationStep() const;
    void setFRotationStep(float fRotationStep);

    float getFRotationStepScale() const;
    void setFRotationStepScale(float fRotationStepScale);

    const std::vector<uint16_t>& getCTHistogramm();
    const std::vector<uint16_t>& getMRHistogramm();



    Core::Math::Vec3f getLeftFBBCenter() const;
    void setLeftFBBCenter(const Core::Math::Vec3f &leftFBBCenter);

    Core::Math::Vec3f getRightFBBCenter() const;
    void setRightFBBCenter(const Core::Math::Vec3f &rightFBBCenter);

    Core::Math::Vec3f getLeftFBBScale() const;
    void setLeftFBBScale(const Core::Math::Vec3f &leftFBBScale);

    Core::Math::Vec3f getRightFBBScale() const;
    void setRightFBBScale(const Core::Math::Vec3f &rightFBBScale);

    uint16_t getMriValue() const;

    Core::Math::Vec3f getMriVolumePosition() const;

    uint16_t getCTValue(Vec3f volumespace);
    uint16_t getMRValue(Vec3f volumespace);

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
    Core::Math::Mat4f                               _CTUnitMatrix;
    Core::Math::Vec3f                               _CTCenter;
    Core::Math::Vec3f                               _CTeX;
    Core::Math::Vec3f                               _CTeY;
    Core::Math::Vec3f                               _CTeZ;

    //CT Frame
    bool                                            _bFoundCTFrame;
    std::vector<Core::Math::Vec3f>                  _leftMarker;
    std::vector<Core::Math::Vec3f>                  _rightMarker;
    Core::Math::Vec3f                               _leftFBBCenter;
    Core::Math::Vec3f                               _rightFBBCenter;
    Core::Math::Vec3f                               _leftFBBScale;
    Core::Math::Vec3f                               _rightFBBScale;


    //Planing Vars
    Core::Math::Vec3f                               _vAC;
    Core::Math::Vec3f                               _vPC;
    Core::Math::Vec3f                               _vMR;

    Core::Math::Vec2i                               _displayedDriveRange;
    Core::Math::Vec2f                               _spectralRange;
    float                                           _MRCTBlend;
    std::vector<Core::Math::Vec3f>                  _FFTColorImage;



    //picking/tracking (store each space, less calculation)
    Core::Math::Vec3f                               _vSelectedVolumeSpacePosition;
    Core::Math::Vec3f                               _vSelectedCTSpacePosition;
    Core::Math::Vec3f                               _vSelectedWorldSpacePositon;

    //status, used for sheduling repaints!
    uint64_t                                        _dataSetStatus;
    uint16_t                                        _mriValue;
    Core::Math::Vec3f                               _mriVolumePosition;

    //networklayer -> soon better
    bool                                            _usesNetworkMER;
    std::unique_ptr<std::thread>                    _networkUpdateThread;

    //Gradient Descent settings
    float                                           _fTranslationStep;
    float                                           _fTranslationStepScale;
    float                                           _fRotationStep;
    float                                           _fRotationStepScale;
};

#endif // DATAHANDLER_H

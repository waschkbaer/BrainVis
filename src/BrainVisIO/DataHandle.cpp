#include <core/FileFinder.h>
#include "DataHandle.h"
#include <algorithm>

DataHandle::DataHandle():
    _transferFunction(nullptr),
    _position(0.5f),
    _gradient(0.5f),
    _MRLoaded(false),
    _CTLoaded(false),
    _MRCTBlend(1.0f),
    _CTeX(1,0,0),
    _CTeY(0,-1,0),
    _CTeZ(0,0,-1),
    _CTCenter(0,0,0),
    _dataSetStatus(0),
    _spectralRange(1000000.0f,-1000000.0f),
    //_MRRotation(-0.1,0.03,0.07),
    //_MROffset(-1.95,6.6501,-31.2998),
    _MRRotation(0,0,0),
    _MROffset(0,0,0),
    _usesNetworkMER(false),
    _fTranslationStep(4.0f),
    _fTranslationStepScale(0.75f),
    _fRotationStep(0.1f),
    _fRotationStepScale(0.75f),
    _leftFBBCenter(0.2f,0.5f,0.5f),
    _rightFBBCenter(0.8f,0.5f,0.5f),
    _leftFBBScale(0.05f,0.25f,0.5f),
    _rightFBBScale(0.05f,0.25f,0.5f),
    _mriValue(0),
    _mriVolumePosition(0,0,0)
{
    incrementStatus();
    createFFTColorImage();
}


void DataHandle::loadMRData(const std::string& path)
{
    _MRVolume = std::unique_ptr<DicomVolume>(new DicomVolume(path));
    _fMRScalingFactor = 65535.0f/_MRVolume->getHistogram().size();
    _transferFunction = std::unique_ptr<DataIO::TransferFunction1D>(new DataIO::TransferFunction1D(_MRVolume->getHistogram().size()));
    _transferFunction->SetStdFunction();

    _MRScale.x = _MRVolume->getAspectRatio().x * _MRVolume->getDimensions().x;
    _MRScale.y = _MRVolume->getAspectRatio().y * _MRVolume->getDimensions().y;
    _MRScale.z = _MRVolume->getAspectRatio().z * _MRVolume->getDimensions().z;

    std::cout << "[DataHandle] MRAspectRatio: "<<_MRVolume->getAspectRatio() <<std::endl;
    std::cout << "[DataHandle] MRDimensions: "<<_MRVolume->getDimensions() <<std::endl;
    std::cout << "[DataHandle] data scale(MR): "<< _MRScale<< std::endl;

    updateMRWorld();
    _MRLoaded = true;
    incrementStatus();
}

void DataHandle::loadCTData(const std::string& path)
{
    _CTVolume = std::unique_ptr<DicomVolume>(new DicomVolume(path));
    _fCTScalingFactor = 65535.0f/_CTVolume->getHistogram().size();
    _transferFunction = std::unique_ptr<DataIO::TransferFunction1D>(new DataIO::TransferFunction1D(_CTVolume->getHistogram().size()));
    _transferFunction->SetStdFunction();

    _CTScale.x = _CTVolume->getAspectRatio().x * _CTVolume->getDimensions().x;
    _CTScale.y = _CTVolume->getAspectRatio().y * _CTVolume->getDimensions().y;
    _CTScale.z = _CTVolume->getAspectRatio().z * _CTVolume->getDimensions().z;

    //cacluate the world space scaling for CT
    std::cout << "[DataHandle] CTAspectRatio: "<<_CTVolume->getAspectRatio() <<std::endl;
    std::cout << "[DataHandle] CTDimensions: "<<_CTVolume->getDimensions() <<std::endl;
    std::cout << "[DataHandle] data scale(CT): "<< _CTScale << std::endl;

    _CTWorld.Scaling(_CTScale);
    _CTLoaded = true;
    incrementStatus();
}

void DataHandle::loadMERFiles(const std::string& path,const std::vector<std::string>& types){
}

void DataHandle::loadMERNetwork(std::vector<std::string> types){

}

void DataHandle::NetworkUpdateThread(){

}

void DataHandle::waitForNetworkThread(){
    _networkUpdateThread->join();
}

std::vector<Core::Math::Vec3f> DataHandle::getFFTColorImage() const
{
    return _FFTColorImage;
}

void DataHandle::createFFTColorImage(){
    Vec3f c;
    float blue,red,green;

    for(int i = 0; i < 600;++i){
        blue =      (-  ((i - 10 )*(i - 10 )    ) *         ((1.0f/(550 ))  *   (1.0f/(550 ))) ) + 1.0f;
        green =     (-  ((i - 300 )*(i - 300 )  ) *         ((1.0f/(290 ))  *   (1.0f/(290 ))) ) + 1.0f;
        red =       (-  ((i - 590 )*(i - 590 )  ) *         ((1.0f/(550 ))  *   (1.0f/(550 ))) ) + 1.0f;

        blue = std::min(1.0f, std::max(blue, 0.0f));
        red = std::min(1.0f, std::max(red, 0.0f));
        green = std::min(1.0f, std::max(green, 0.0f));

        c = Vec3f(red,green,blue);
        _FFTColorImage.push_back(c);
    }
}

Core::Math::Vec2i DataHandle::getDisplayedDriveRange() const
{
    return _displayedDriveRange;
}

void DataHandle::setDisplayedDriveRange(const Core::Math::Vec2i &displayedDriveRange)
{
    _displayedDriveRange = displayedDriveRange;
    incrementStatus();
}

void DataHandle::updateMRWorld(){
    Mat4f scale;
    Mat4f scaleInv;
    Mat4f trans;
    Mat4f rotX,rotY,rotZ;
    
    scale.Scaling(_MRScale);
    trans.Translation((Vec3f(_MROffset.x,_MROffset.y,_MROffset.z)));
    rotX.RotationX(_MRRotation.x);
    rotY.RotationY(_MRRotation.y);
    rotZ.RotationZ(_MRRotation.z);


    scaleInv = scale.inverse();

    _MRWorld = scale*rotX*rotY*rotZ*trans;

    incrementStatus();
}
Core::Math::Vec3f DataHandle::getMriVolumePosition() const
{
    return _mriVolumePosition;
}

uint16_t DataHandle::getMriValue() const
{
    return _mriValue;
}

Core::Math::Vec3f DataHandle::getRightFBBScale() const
{
    return _rightFBBScale;
}

void DataHandle::setRightFBBScale(const Core::Math::Vec3f &rightFBBScale)
{
    _rightFBBScale = rightFBBScale;
    incrementStatus();
}

Core::Math::Vec3f DataHandle::getLeftFBBScale() const
{
    return _leftFBBScale;
}

void DataHandle::setLeftFBBScale(const Core::Math::Vec3f &leftFBBScale)
{
    _leftFBBScale = leftFBBScale;
    incrementStatus();
}

Core::Math::Vec3f DataHandle::getRightFBBCenter() const
{
    return _rightFBBCenter;
}

void DataHandle::setRightFBBCenter(const Core::Math::Vec3f &rightFBBCenter)
{
    _rightFBBCenter = rightFBBCenter;
    incrementStatus();
}

Core::Math::Vec3f DataHandle::getLeftFBBCenter() const
{
    return _leftFBBCenter;
}

void DataHandle::setLeftFBBCenter(const Core::Math::Vec3f &leftFBBCenter)
{
    _leftFBBCenter = leftFBBCenter;
    incrementStatus();
}

float DataHandle::getGradient() const
{
    return _gradient;
}

float DataHandle::getPosition() const
{
    return _position;
}

float DataHandle::getFRotationStepScale() const
{
    return _fRotationStepScale;
}

void DataHandle::setFRotationStepScale(float fRotationStepScale)
{
    _fRotationStepScale = fRotationStepScale;
}

float DataHandle::getFRotationStep() const
{
    return _fRotationStep;
}

void DataHandle::setFRotationStep(float fRotationStep)
{
    _fRotationStep = fRotationStep;
}

float DataHandle::getFTranslationStepScale() const
{
    return _fTranslationStepScale;
}

void DataHandle::setFTranslationStepScale(float fTranslationStepScale)
{
    _fTranslationStepScale = fTranslationStepScale;
}

float DataHandle::getFTranslationStep() const
{
    return _fTranslationStep;
}

void DataHandle::setFTranslationStep(float fTranslationStep)
{
    _fTranslationStep = fTranslationStep;
}


uint64_t DataHandle::getDataSetStatus() const
{
    return _dataSetStatus;
}

Core::Math::Vec2f DataHandle::getSpectralRange() const
{
    return _spectralRange;
}

Core::Math::Vec3ui DataHandle::getMRDimensions() const { return _MRVolume->getDimensions();}
Core::Math::Vec3ui DataHandle::getCTDimensions() const { return _CTVolume->getDimensions();}

const std::vector<uint16_t>&      DataHandle::getMRData() {return _MRVolume->getData(); }
const std::vector<uint16_t>&      DataHandle::getCTData() {return _CTVolume->getData(); }
Core::Math::Vec3f       DataHandle::getMRAspectRatio() {return _MRVolume->getAspectRatio();}
Core::Math::Vec3f       DataHandle::getCTAspectRatio() {return _CTVolume->getAspectRatio();}

float DataHandle::getMRCTBlend() const
{
    return _MRCTBlend;
}

void DataHandle::setMRCTBlend(float MRCTBlend)
{
    _MRCTBlend = MRCTBlend;
    _MRCTBlend = std::max(0.0f, std::min(1.0f,_MRCTBlend));
    incrementStatus();
}

Core::Math::Vec3f DataHandle::getMRRotation() const
{
    return _MRRotation;
}

void DataHandle::setMRRotation(const Core::Math::Vec3f &MRRotation)
{
    _MRRotation = MRRotation;
    updateMRWorld();
    incrementStatus();
}


Core::Math::Vec3f DataHandle::getSelectedSlices() const
{
    return _vSelectedVolumeSpacePosition;
}
void DataHandle::setSelectedSlices(const Core::Math::Vec3f slides)
{
    _vSelectedVolumeSpacePosition =slides;
    _vSelectedVolumeSpacePosition.x = std::max(0.0f, std::min(1.0f, _vSelectedVolumeSpacePosition.x));
    _vSelectedVolumeSpacePosition.y = std::max(0.0f, std::min(1.0f, _vSelectedVolumeSpacePosition.y));
    _vSelectedVolumeSpacePosition.z =  std::max(0.0f, std::min(1.0f, _vSelectedVolumeSpacePosition.z));

    Mat4f s;
    Vec3f scal;
    scal.x = getCTAspectRatio().x*getCTDimensions().x;
    scal.y = getCTAspectRatio().y*getCTDimensions().y;
    scal.z = getCTAspectRatio().z*getCTDimensions().z;

    s.Scaling(scal);

    Vec4f worldSpacePosition = s* (Vec4f(_vSelectedVolumeSpacePosition.x,
                                         _vSelectedVolumeSpacePosition.y,
                                         _vSelectedVolumeSpacePosition.z,
                                         1)
                                         -Vec4f(0.5,0.5,0.5,0.0));
    Vec4f worldSpaceCenter = s * (Vec4f(getCTCenter().x,getCTCenter().y,getCTCenter().z,1));
    Vec3f distance = worldSpacePosition.xyz() - worldSpaceCenter.xyz();
    _vSelectedCTSpacePosition =  (      distance.x * getCTeX() +
                                        distance.y * getCTeY() +
                                        distance.z * getCTeZ())+ Vec3f(100,100,100);
    _vSelectedWorldSpacePositon = worldSpacePosition.xyz();

    //calc mr volumespace
    Mat4f invWorld = this->getMRWorld().inverse();
    Vec4f mrSpace = worldSpacePosition*invWorld;
    _mriValue = _MRVolume->getValue(Vec3f(mrSpace.x+0.5f,mrSpace.y+0.5f,mrSpace.z+0.5f));
    _mriVolumePosition = Vec3f(mrSpace.x+0.5f,mrSpace.y+0.5f,mrSpace.z+0.5f);

    incrementStatus();
}

float DataHandle::getMRTransferScaling() const
{
    return _fMRScalingFactor;
}
float DataHandle::getCTTransferScaling() const
{
    return _fCTScalingFactor;
}

void DataHandle::setSmoothStep(float pos, float grad){
    _transferFunction->SetStdFunction(pos,grad);
    _position = pos;
    _gradient = grad;
    incrementStatus();
}
std::string DataHandle::getMRPath() const
{
    return _MRPath;
}

void DataHandle::setMRPath(const std::string &MRPath)
{
    _MRPath = MRPath;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getMROffset() const
{
    return _MROffset;

}

void DataHandle::setMROffset(const Core::Math::Vec3f &MROffset)
{
    _MROffset = MROffset;

    updateMRWorld();
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getMRScale() const
{
    return _MRScale;
}

void DataHandle::setMRScale(const Core::Math::Vec3f &MRScale)
{
    _MRScale = MRScale;
    updateMRWorld();
    incrementStatus();
}
float DataHandle::getFMRScalingFactor() const
{
    return _fMRScalingFactor;
}

void DataHandle::setFMRScalingFactor(float fMRScalingFactor)
{
    _fMRScalingFactor = fMRScalingFactor;
    incrementStatus();
}
std::string DataHandle::getCTPath() const
{
    return _CTPath;
}

void DataHandle::setCTPath(const std::string &CTPath)
{
    _CTPath = CTPath;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getCTScale() const
{
    return _CTScale;
}

void DataHandle::setCTScale(const Core::Math::Vec3f &CTScale)
{
    _CTScale = CTScale;
    _CTWorld.Scaling(_CTScale);
    incrementStatus();
}
float DataHandle::getFCTScalingFactor() const
{
    return _fCTScalingFactor;
}

void DataHandle::setFCTScalingFactor(float fCTScalingFactor)
{
    _fCTScalingFactor = fCTScalingFactor;
    incrementStatus();
}
bool DataHandle::getBFoundCTFrame() const
{
    return _bFoundCTFrame;
}

void DataHandle::setBFoundCTFrame(bool bFoundCTFrame)
{
    _bFoundCTFrame = bFoundCTFrame;
    incrementStatus();
}
std::vector<Core::Math::Vec3f> DataHandle::getLeftMarker() const
{
    return _leftMarker;
}

void DataHandle::setLeftMarker(const std::vector<Core::Math::Vec3f> &leftMarker)
{
    _leftMarker = leftMarker;
    incrementStatus();
}
std::vector<Core::Math::Vec3f> DataHandle::getRightMarker() const
{
    return _rightMarker;
}

void DataHandle::setRightMarker(const std::vector<Core::Math::Vec3f> &rightMarker)
{
    _rightMarker = rightMarker;
    incrementStatus();
}
Core::Math::Mat4f DataHandle::getCTUnitMatrix() const
{
    return _CTUnitMatrix;
}
Core::Math::Vec3f DataHandle::getCTCenter() const
{
    return _CTCenter;
}

void DataHandle::setCTCenter(const Core::Math::Vec3f &CTCenter)
{
    _CTCenter = CTCenter;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getAC() const
{
    return _vAC;
}

void DataHandle::setAC(const Core::Math::Vec3f &vAC)
{
    _vAC = vAC;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getPC() const
{
    return _vPC;
}

void DataHandle::setPC(const Core::Math::Vec3f &vPC)
{
    _vPC = vPC;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getMR() const
{
    return _vMR;
}

void DataHandle::setMR(const Core::Math::Vec3f &vMR)
{
    _vMR = vMR;
    incrementStatus();
}

Core::Math::Vec3f DataHandle::getCTeX() const
{
    return _CTeX;
}

void DataHandle::setCTeX(const Core::Math::Vec3f &CTeX)
{
    _CTeX = CTeX;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getCTeY() const
{
    return _CTeY;
}

void DataHandle::setCTeY(const Core::Math::Vec3f &CTeY)
{
    _CTeY = CTeY;
    incrementStatus();
}
Core::Math::Vec3f DataHandle::getCTeZ() const
{
    return _CTeZ;
}

void DataHandle::setCTeZ(const Core::Math::Vec3f &CTeZ)
{
    _CTeZ = CTeZ;
    incrementStatus();
}

Core::Math::Mat4f DataHandle::getMRWorld() const
{
    return _MRWorld;
}

void DataHandle::setMRWorld(const Core::Math::Mat4f &MRWorld)
{
    _MRWorld = MRWorld;
    incrementStatus();
}
Core::Math::Mat4f DataHandle::getCTWorld() const
{
    return _CTWorld;
}

void DataHandle::setCTWorld(const Core::Math::Mat4f &CTWorld)
{
    _CTWorld = CTWorld;
    incrementStatus();
}

std::shared_ptr<std::vector<Core::Math::Vec4f>> DataHandle::getTransferFunction()
{
    return _transferFunction->GetColorData();
}

Core::Math::Vec3f DataHandle::getCTOffset() const
{
    return _CTOffset;
}

void DataHandle::setCTOffset(const Core::Math::Vec3f &CTOffset)
{
    _CTOffset = CTOffset;
    incrementStatus();
}
bool DataHandle::getMRLoaded() const
{
    return _MRLoaded;
}

void DataHandle::setMRLoaded(bool MRLoaded)
{
    _MRLoaded = MRLoaded;
    incrementStatus();
}
bool DataHandle::getCTLoaded() const
{
    return _CTLoaded;
}

void DataHandle::setCTLoaded(bool CTLoaded)
{
    _CTLoaded = CTLoaded;
    incrementStatus();
}




bool DataHandle::calculateCTUnitVectors(){
    if(_leftMarker.size() != 4 || _rightMarker.size() != 4){
        return false;
    }


    //derp!!
    Vec3f d = _leftMarker[3]-_leftMarker[0]+_leftMarker[2]-_leftMarker[1]+_rightMarker[3]-_rightMarker[0]+_rightMarker[2]-_rightMarker[1];
    d /= 4.0f;
    std::cout <<"[DataHandle] testD L : "<< d.length() << " in worldspace avg "<< (_CTScale*d).length()<<std::endl;


    /*//test length
    std::cout << "[DataHandle] Length X l0r0 : "<< (_CTScale*_rightMarker[0]-_CTScale*_leftMarker[0]).length() <<std::endl;
    std::cout << "[DataHandle] Length X l1r1 : "<< (_CTScale*_rightMarker[1]-_CTScale*_leftMarker[1]).length() <<std::endl;
    std::cout << "[DataHandle] Length X l2r2 : "<< (_CTScale*_rightMarker[2]-_CTScale*_leftMarker[2]).length() <<std::endl;
    std::cout << "[DataHandle] Length X l3r3 : "<< (_CTScale*_rightMarker[3]-_CTScale*_leftMarker[3]).length() <<std::endl;

    std::cout << "[DataHandle] Length Z l0l3 : "<< (_CTScale*_leftMarker[3]-_CTScale*_leftMarker[0]).length() <<std::endl;
    std::cout << "[DataHandle] Length Z l1l2 : "<< (_CTScale*_leftMarker[2]-_CTScale*_leftMarker[1]).length() <<std::endl;
    std::cout << "[DataHandle] Length Z r0r3 : "<< (_CTScale*_rightMarker[3]-_CTScale*_rightMarker[0]).length() <<std::endl;
    std::cout << "[DataHandle] Length Z r1r3 : "<< (_CTScale*_rightMarker[2]-_CTScale*_rightMarker[1]).length() <<std::endl;

    std::cout << "[DataHandle] Length Y l1l0 : "<< (_CTScale*_leftMarker[0]-_CTScale*_leftMarker[1]).length() <<std::endl;
    std::cout << "[DataHandle] Length Y l2l3 : "<< (_CTScale*_leftMarker[3]-_CTScale*_leftMarker[2]).length() <<std::endl;
    std::cout << "[DataHandle] Length Y r1r0 : "<< (_CTScale*_rightMarker[0]-_CTScale*_rightMarker[1]).length() <<std::endl;
    std::cout << "[DataHandle] Length Y r2r3 : "<< (_CTScale*_rightMarker[3]-_CTScale*_rightMarker[2]).length() <<std::endl;
    */

    Mat4f worldScaling;
    worldScaling.Scaling(_CTScale);

    _CTeX = (_CTScale*_rightMarker[0]-_CTScale*_leftMarker[0]+_CTScale*_rightMarker[1]-_CTScale*_leftMarker[1]+_CTScale*_rightMarker[2]-_CTScale*_leftMarker[2]+_CTScale*_rightMarker[3]-_CTScale*_leftMarker[3]);
    _CTeX.normalize();

    _CTeZ = _CTScale*_leftMarker[3]-_CTScale*_leftMarker[0]+_CTScale*_leftMarker[2]-_CTScale*_leftMarker[1]+_CTScale*_rightMarker[3]-_CTScale*_rightMarker[0]+_CTScale*_rightMarker[2]-_CTScale*_rightMarker[1];
    _CTeZ.normalize();

    _CTeY = _CTScale*_leftMarker[0]-_CTScale*_leftMarker[1]+_CTScale*_leftMarker[3]-_CTScale*_leftMarker[2]+_CTScale*_rightMarker[0]-_CTScale*_rightMarker[1]+_CTScale*_rightMarker[3]-_CTScale*_rightMarker[2];
    _CTeY.normalize();

    //center is equal to (100,100,100)
    _CTCenter = ((_leftMarker[1]+ 0.5f*(_rightMarker[3]-_leftMarker[1])) +
              (_leftMarker[0]+ 0.5f*(_rightMarker[2]-_leftMarker[0])) +
              (_leftMarker[2]+ 0.5f*(_rightMarker[0]-_leftMarker[2])) +
              (_leftMarker[3]+ 0.5f*(_rightMarker[1]-_leftMarker[3])) )/4.0f;

    Vec4f worldcenter = (worldScaling*Vec4f(_CTCenter,1));

    _CTOffset = -_CTCenter;

    //debug out!
    std::cout << "[DataHandle] ------------------"<<std::endl;
    std::cout << "[DataHandle] Center (volume Space) " << _CTCenter << std::endl;
    std::cout << "[DataHandle] Center (world Space) " << worldcenter << std::endl;
    std::cout << "[DataHandle] ------------------"<<std::endl;
    std::cout << "[DataHandle] Ex: " << _CTeX << std::endl;
    std::cout << "[DataHandle] Ey: " << _CTeY << std::endl;
    std::cout << "[DataHandle] Ez: " << _CTeZ << std::endl;
    std::cout << "[DataHandle] ------------------"<<std::endl;

    incrementStatus();
    return true;
}

void DataHandle::incrementStatus(){
    _dataSetStatus++;
}
Core::Math::Vec3f DataHandle::getSelectedWorldSpacePositon() const
{
    return _vSelectedWorldSpacePositon;
}

Core::Math::Vec3f DataHandle::getSelectedCTSpacePosition() const
{
    return _vSelectedCTSpacePosition;
}

bool DataHandle::getUsesNetworkMER() const
{
    return _usesNetworkMER;
}

void DataHandle::setFocusPoint(Core::Math::Vec3f position){

    Vec3f worldSpace = position;
    Vec3f volumeSpace = (worldSpace/_CTScale)+Vec3f(0.5f,0.5f,0.5f);

    if(volumeSpace != getSelectedSlices()){
        setSelectedSlices(volumeSpace);
        incrementStatus();
    }

}

const std::vector<uint16_t>& DataHandle::getCTHistogramm(){
    std::vector<uint16_t> h;
    if(_CTVolume != nullptr)
        return _CTVolume->getHistogram();
    else
        return h;
}

const std::vector<uint16_t>& DataHandle::getMRHistogramm(){
    std::vector<uint16_t> h;
    if(_MRVolume != nullptr)
        return _MRVolume->getHistogram();
    else
        return h;
}

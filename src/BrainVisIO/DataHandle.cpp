#include "DataHandle.h"
#include <algorithm>

#include "MER-Data/FileElectrode.h"
#include "MER-Data/NetworkElectrode.h"
#include "MER-Data/NetworkMERData.h"

DataHandle::DataHandle():
    _transferFunction(nullptr),
    _leftSTN(),
    _rightSTN(),
    _MRLoaded(false),
    _CTLoaded(false),
    _MRCTBlend(1.0f),
    _CTeX(1,0,0),
    _CTeY(0,-1,0),
    _CTeZ(0,0,-1),
    _dataSetStatus(0),
    _spectralRange(1000000.0f,-1000000.0f),
    _MRRotation(-0.1,0.03,0.07),
    _MROffset(-1.95,6.6501,-31.2998),
    //_MRRotation(0,0,0),
    //_MROffset(0,0,0),
    _usesNetworkMER(false)
{
    incrementStatus();
    createFFTColorImage();

    MERConnection::getInstance().clear();
    ElectrodeManager::getInstance().clear();

    MERConnection::getInstance().openConnection("localhost",41234);
}


void DataHandle::loadMRData(std::string& path)
{
    _MRVolume = std::unique_ptr<DicomVolume>(new DicomVolume(path));
    _fMRScalingFactor = 65535.0f/_MRVolume->getHistogram().size();
    _transferFunction = std::unique_ptr<DataIO::TransferFunction1D>(new DataIO::TransferFunction1D(_MRVolume->getHistogram().size()));
    _transferFunction->SetStdFunction();

    _MRScale.x = _MRVolume->getAspectRatio().x * _MRVolume->getDimensions().x;
    _MRScale.y = _MRVolume->getAspectRatio().y * _MRVolume->getDimensions().y;
    _MRScale.z = _MRVolume->getAspectRatio().z * _MRVolume->getDimensions().z;

    std::cout << "MRAspectRatio: "<<_MRVolume->getAspectRatio() <<std::endl;
    std::cout << "MRDimensions: "<<_MRVolume->getDimensions() <<std::endl;
    std::cout << "data scale(MR): "<< _MRScale<< std::endl;

    updateMRWorld();
    _MRLoaded = true;
    incrementStatus();
}

void DataHandle::loadCTData(std::string& path)
{
    _CTVolume = std::unique_ptr<DicomVolume>(new DicomVolume(path));
    _fCTScalingFactor = 65535.0f/_CTVolume->getHistogram().size();
    _transferFunction = std::unique_ptr<DataIO::TransferFunction1D>(new DataIO::TransferFunction1D(_CTVolume->getHistogram().size()));
    _transferFunction->SetStdFunction();

    _CTScale.x = _CTVolume->getAspectRatio().x * _CTVolume->getDimensions().x;
    _CTScale.y = _CTVolume->getAspectRatio().y * _CTVolume->getDimensions().y;
    _CTScale.z = _CTVolume->getAspectRatio().z * _CTVolume->getDimensions().z;

    //cacluate the world space scaling for CT
    std::cout << "CTAspectRatio: "<<_CTVolume->getAspectRatio() <<std::endl;
    std::cout << "CTDimensions: "<<_CTVolume->getDimensions() <<std::endl;
    std::cout << "data scale(CT): "<< _CTScale << std::endl;

    _CTWorld.Scaling(_CTScale);
    _CTLoaded = true;
    incrementStatus();
}

void DataHandle::loadMERFiles(std::string& path,std::vector<std::string> types){
    std::vector<std::string> csvs;
    Core::FileFinder::getInstance().readFilesWithEnding(path,csvs,".csv");

    std::vector<std::string> positionList;
    std::vector<std::string> spectralList;
    std::string spec;

    for(int i = 0; i < types.size();++i){
        spec = types[i]+"Spec";
        for(int j = 0; j < csvs.size();++j){
            if(csvs[j].find(types[i]) != std::string::npos &&
               csvs[j].find(spec) == std::string::npos){

                positionList.push_back(csvs[j]);

            }else if(csvs[j].find(spec) != std::string::npos){
                spectralList.push_back(csvs[j]);

            }
        }
    }

    for(int i = 0; i < positionList.size();++i){
        std::shared_ptr<iElectrode> elec = std::make_shared<FileElectrode>(types[i], Core::Math::Vec2d(250,1000),positionList[i],spectralList[i]);

        ElectrodeManager::getInstance().addElectrode(elec);

        _spectralRange.x = std::min((float)elec->getSpectralPowerRange().x, _spectralRange.x);
        _spectralRange.y = std::max((float)elec->getSpectralPowerRange().y, _spectralRange.y);
    }
}

void DataHandle::loadMERNetwork(std::vector<std::string> types){
    _usesNetworkMER = true;
    std::vector<std::string> registeredElectrodes = MERConnection::getInstance().getRegisteredElectrodes();

    for(int i = 0; i < registeredElectrodes.size();++i){
        std::shared_ptr<iElectrode> elec = std::make_shared<NetworkElectrode>(registeredElectrodes[i]);
        int curDepth = MERConnection::getInstance().getCurrentDepth(registeredElectrodes[i]);

        for(int j = -10; j <= curDepth;j++){
            std::shared_ptr<iMERData> data = std::make_shared<NetworkMERData>();
            data->setDataPosition(MERConnection::getInstance().getPosition(registeredElectrodes[i],j));
            data->setFrequency(2000.0f);
            data->setFrequencyRange(Core::Math::Vec2d(0,2000.0f));
            data->setInput(MERConnection::getInstance().getSignal(registeredElectrodes[i],j));
            data->executeFFT();

            elec->addData(j,data);
        }

        ElectrodeManager::getInstance().addElectrode(elec);
    }

    _networkUpdateThread = std::unique_ptr<std::thread>(new std::thread(&DataHandle::NetworkUpdateThread,this));
}

void DataHandle::NetworkUpdateThread(){
    bool isConnected = MERConnection::getInstance().getIsConnected();
    bool changed = false;
    int curDepth = 0;
    std::shared_ptr<iElectrode> elec;
    std::shared_ptr<iMERData> data;
    while(isConnected){
        isConnected = MERConnection::getInstance().getIsConnected();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        changed = false;

       for(int i = 0; i < ElectrodeManager::getInstance().getElectrodeCount();++i){
            elec = ElectrodeManager::getInstance().getElectrode(i);
            curDepth = MERConnection::getInstance().getCurrentDepth(elec->getName());
            if(curDepth > elec->getDepthRange().y){
                changed = true;
                for(int j = elec->getDepthRange().y+1; j <= curDepth;j++ ){
                    data = std::make_shared<NetworkMERData>();
                    data->setDataPosition(MERConnection::getInstance().getPosition(elec->getName(),j));
                    data->setFrequency(2000.0f);
                    data->setFrequencyRange(Core::Math::Vec2d(0,2000.0f));
                    data->setInput(MERConnection::getInstance().getSignal(elec->getName(),j));
                    data->executeFFT();

                    elec->addData(j,data);
                }
            }
       }
       if(changed){
         incrementStatus();
       }
    }
}

void DataHandle::waitForNetworkThread(){
    MERConnection::getInstance().clear();
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
uint64_t DataHandle::getDataSetStatus() const
{
    return _dataSetStatus;
}

Core::Math::Vec2f DataHandle::getSpectralRange() const
{
    return _spectralRange;
}


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
void DataHandle::setSelectedSlices(Core::Math::Vec3f slides)
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
Trajectory DataHandle::getLeftSTN() const
{
    return _leftSTN;
}

void DataHandle::setLeftSTN(const Trajectory &leftSTN)
{
    _leftSTN = leftSTN;
    incrementStatus();
}
Trajectory DataHandle::getRightSTN() const
{
    return _rightSTN;
}

void DataHandle::setRightSTN(const Trajectory &rightSTN)
{
    _rightSTN = rightSTN;
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

    Mat4f worldScaling;
    worldScaling.Scaling(_CTScale);

    _CTeX = (_rightMarker[0]-_leftMarker[0]+_rightMarker[1]-_leftMarker[1]+_rightMarker[2]-_leftMarker[2]+_rightMarker[3]-_leftMarker[3]);
    _CTeX.normalize();

    _CTeZ = _leftMarker[3]-_leftMarker[0]+_leftMarker[2]-_leftMarker[1]+_rightMarker[3]-_rightMarker[0]+_rightMarker[2]-_rightMarker[1];
    _CTeZ.normalize();

    _CTeY = _leftMarker[0]-_leftMarker[1]+_leftMarker[3]-_leftMarker[2]+_rightMarker[0]-_rightMarker[1]+_rightMarker[3]-_rightMarker[2];
    _CTeY.normalize();

    //center is equal to (100,100,100)
    _CTCenter = ((_leftMarker[1]+ 0.5f*(_rightMarker[3]-_leftMarker[1])) +
              (_leftMarker[0]+ 0.5f*(_rightMarker[2]-_leftMarker[0])) +
              (_leftMarker[2]+ 0.5f*(_rightMarker[0]-_leftMarker[2])) +
              (_leftMarker[3]+ 0.5f*(_rightMarker[1]-_leftMarker[3])) )/4.0f;

    Vec4f worldcenter = (worldScaling*Vec4f(_CTCenter,1));

    Trajectory leftSTN(_leftSTN._startPlaning,_leftSTN._endPlaning);
    Trajectory rightSTN(_rightSTN._startPlaning,_rightSTN._endPlaning);


    //calculate STN World Space Position!
    Vec3f leftSTNDistanceEntry = leftSTN._startPlaning-Vec3f(100,100,100);
    Vec3f rightSTNDistanceEntry = rightSTN._startPlaning-Vec3f(100,100,100);
    Vec3f leftSTNDistance = leftSTN._endPlaning-Vec3f(100,100,100);
    Vec3f rightSTNDistance = rightSTN._endPlaning-Vec3f(100,100,100);

    leftSTN._endWorldSpace = worldcenter.xyz() + leftSTNDistance.x * _CTeX + leftSTNDistance.y * _CTeY + leftSTNDistance.z * _CTeZ;
    rightSTN._endWorldSpace = worldcenter.xyz() + rightSTNDistance.x * _CTeX + rightSTNDistance.y * _CTeY + rightSTNDistance.z * _CTeZ;

    leftSTN._startWorldSpace = worldcenter.xyz() + leftSTNDistanceEntry.x * _CTeX + leftSTNDistanceEntry.y * _CTeY + leftSTNDistanceEntry.z * _CTeZ;
    rightSTN._startWorldSpace = worldcenter.xyz() + rightSTNDistanceEntry.x * _CTeX + rightSTNDistanceEntry.y * _CTeY + rightSTNDistanceEntry.z * _CTeZ;

    leftSTN._endVolumeSpace = (Vec4f(leftSTN._endWorldSpace,1)*worldScaling.inverse()).xyz()+Vec3f(0.5f,0.5f,0.5f);
    rightSTN._endVolumeSpace = (Vec4f(rightSTN._endWorldSpace,1)*worldScaling.inverse()).xyz()+Vec3f(0.5f,0.5f,0.5f);
    leftSTN._startVolumeSpace = (Vec4f(leftSTN._startWorldSpace,1)*worldScaling.inverse()).xyz()+Vec3f(0.5f,0.5f,0.5f);
    rightSTN._startVolumeSpace = (Vec4f(rightSTN._startWorldSpace,1)*worldScaling.inverse()).xyz()+Vec3f(0.5f,0.5f,0.5f);


    _leftSTN = leftSTN;
    _rightSTN = rightSTN;

    _CTOffset = -_CTCenter;

    //debug out!
    std::cout << "------------------"<<std::endl;
    std::cout << "Center (volume Space) " << _CTCenter << std::endl;
    std::cout << "Center (world Space) " << worldcenter << std::endl;
    std::cout << "------------------"<<std::endl;
    std::cout << "Ex: " << _CTeX << std::endl;
    std::cout << "Ey: " << _CTeY << std::endl;
    std::cout << "Ez: " << _CTeZ << std::endl;
    std::cout << "------------------"<<std::endl;
    std::cout << "target left : "<< leftSTN._endWorldSpace <<std::endl;
    std::cout << "target right : "<< rightSTN._endWorldSpace <<std::endl;
    std::cout << "entry left : "<< leftSTN._startWorldSpace <<std::endl;
    std::cout << "entry right : "<< rightSTN._startWorldSpace <<std::endl;
    std::cout << "------------------"<<std::endl;
    std::cout << "target leftvs : "<< leftSTN._endVolumeSpace <<std::endl;
    std::cout << "target rightvs : "<< rightSTN._endVolumeSpace <<std::endl;
    std::cout << "entry leftvs : "<< leftSTN._startVolumeSpace <<std::endl;
    std::cout << "entry rightvs : "<< rightSTN._startVolumeSpace <<std::endl;
    std::cout << "------------------"<<std::endl;

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


std::shared_ptr<iElectrode> DataHandle::getElectrode(std::string name){
    return ElectrodeManager::getInstance().getElectrode(name);
}

std::shared_ptr<iElectrode> DataHandle::getElectrode(int i){
    if(i >= 0 && i < ElectrodeManager::getInstance().getElectrodeCount()){
        return ElectrodeManager::getInstance().getElectrode(i);
    }else{
        return nullptr;
    }
}

void DataHandle::checkFocusPoint(){
    if(ElectrodeManager::getInstance().getTrackedElectrode() != "none"){
        std::shared_ptr<iElectrode> elec = ElectrodeManager::getInstance().getElectrode(ElectrodeManager::getInstance().getTrackedElectrode());
        if(elec == nullptr) return;

        Vec3f CTSpacePosition = elec->getData(getDisplayedDriveRange().y)->getDataPosition();

        CTSpacePosition = CTSpacePosition-Vec3f(100,100,100);
        Vec3f worldSpace =  getCTeX()*CTSpacePosition.x +
                            getCTeY()*CTSpacePosition.y +
                            getCTeZ()*CTSpacePosition.z;
        Vec3f volumeSpace = (worldSpace/_CTScale)+this->getCTCenter()+Vec3f(0.5f,0.5f,0.5f);

        if(volumeSpace != getSelectedSlices()){
            setSelectedSlices(volumeSpace);
            incrementStatus();
        }
    }
}

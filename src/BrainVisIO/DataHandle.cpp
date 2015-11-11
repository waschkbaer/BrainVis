#include "DataHandle.h"
#include <algorithm>

#include "MER-Data/FileElectrode.h"

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
    _spectralRange(1000.0f,-1000.0f),
    _MRRotation(-0.1,0,0.1),
    _MROffset(-0.005,-0.74,0.115)
{
    incrementStatus();
    createFFTColorImage();
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
                std::cout <<"posfile"<< csvs[j] << std::endl;

            }else if(csvs[j].find(spec) != std::string::npos){
                std::cout <<"specfile"<< csvs[j] << std::endl;
                spectralList.push_back(csvs[j]);

            }
        }
    }

    for(int i = 0; i < positionList.size();++i){
        std::shared_ptr<iElectrode> elec = std::make_shared<FileElectrode>(types[i],positionList[i],spectralList[i]);
        _electrodeData.addElectrode(elec);
    }
    for(int i = 0; i < types.size();++i){
        std::cout << "MERINFO: " <<_electrodeData.getElectrode(types[i])->getName() << std::endl;
        std::cout << "MERINFO: " <<_electrodeData.getElectrode(types[i])->getSpectralPowerRange() << std::endl;
    }
}

void DataHandle::calculateSpectralRange(){
   /* for(int i = 0; i < _leftBundle->getTrajectoryCount();++i){
        for(int j = -10; j <= 4;++j){
            _spectralRange.x = std::min(_spectralRange.x, _leftBundle->getTrajectory(i)->getData(j)->spectralRange().x);
            _spectralRange.y = std::max(_spectralRange.y, _leftBundle->getTrajectory(i)->getData(j)->spectralRange().y);

        }
    }

    for(int i = 0; i < _rightBundle->getTrajectoryCount();++i){
        for(int j = -10; j <= 4;++j){
            _spectralRange.x = std::min(_spectralRange.x, _rightBundle->getTrajectory(i)->getData(j)->spectralRange().x);
            _spectralRange.y = std::max(_spectralRange.y, _rightBundle->getTrajectory(i)->getData(j)->spectralRange().y);
        }
    }*/
}

std::vector<Core::Math::Vec3f> DataHandle::getFFTColorImage() const
{
    return _FFTColorImage;
}

void DataHandle::createFFTColorImage(){
    Vec3f c;
    float blue,red,green;
   /* for(int i = 0; i < 100;++i){
         blue     = (50.0f - i) / 50.0f;
         red      = (i-49.0f) / 49.0f;
         if(i < 50){
            green    = ( i ) / 60.0f;
         }else{
            green = 1.0f - (50.0f - i ) / 50.0f;
         }
         blue = std::min(1.0f, std::max(blue, 0.0f));
         red = std::min(1.0f, std::max(red, 0.0f));
         green = std::min(1.0f, std::max(green, 0.0f));
         c = Vec3f(red,green,blue);
         //std::cout << c << std::endl;
         _FFTColorImage.push_back(c);
    }*/

    //formel -> y = (-((x - center )^2) * stauchung ) +maximum
    for(int i = 0; i < 600;++i){
        blue =      (-  ((i - 10 )*(i - 10 )    ) *         ((1.0f/(550 ))  *   (1.0f/(550 ))) ) + 1.0f;
        green =     (-  ((i - 300 )*(i - 300 )  ) *         ((1.0f/(290 ))  *   (1.0f/(290 ))) ) + 1.0f;
        red =       (-  ((i - 590 )*(i - 590 )  ) *         ((1.0f/(550 ))  *   (1.0f/(550 ))) ) + 1.0f;

        blue = std::min(1.0f, std::max(blue, 0.0f));
        red = std::min(1.0f, std::max(red, 0.0f));
        green = std::min(1.0f, std::max(green, 0.0f));

        c = Vec3f(red,green,blue);
        //std::cout << c << std::endl;
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
    Mat4f trans;
    Mat4f rotX,rotY,rotZ;
    
    scale.Scaling(_MRScale);
    trans.Translation(0.5f*(Vec3f(-_MROffset.x,_MROffset.z,_MROffset.y)*Vec3f(_MRScale.x,_MRScale.y,_MRScale.z)));
    rotX.RotationX(_MRRotation.x);
    rotY.RotationY(_MRRotation.y);
    rotZ.RotationZ(_MRRotation.z);

    _MRWorld = scale*trans*rotX*rotY*rotZ;
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
    std::cout << "MR ROTATION << "<< _MRRotation <<std::endl;
    updateMRWorld();
    incrementStatus();
}


Core::Math::Vec3f DataHandle::getSelectedSlices() const
{
    return _vSliceSelection;
}
void DataHandle::setSelectedSlices(Core::Math::Vec3f slides)
{
    _vSliceSelection =slides;
    _vSliceSelection.x = std::max(0.0f, std::min(1.0f, _vSliceSelection.x));
    _vSliceSelection.y = std::max(0.0f, std::min(1.0f, _vSliceSelection.y));
    _vSliceSelection.z =  std::max(0.0f, std::min(1.0f, _vSliceSelection.z));
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
    std::cout << "MR OFFSET << "<< _MROffset<< "MR SCALE : " << _MRScale << std::endl;
    std::cout << "TRANSLATION: "<< 0.5f*(Vec3f(-_MROffset.x,_MROffset.z,_MROffset.y)*Vec3f(_MRScale.x,_MRScale.y,_MRScale.z)) << std::endl;

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
std::cout << "LOADING CENTER "<< worldcenter << std::endl;
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

std::shared_ptr<iElectrode> DataHandle::getElectrode(std::string name){
    return _electrodeData.getElectrode(name);
}
std::shared_ptr<iElectrode> DataHandle::getElectrode(int i){
    if(i >= 0 && i < _electrodeData.getElectrodeCount()){
        _electrodeData.getElectrode(i);
    }else{
        return nullptr;
    }
}

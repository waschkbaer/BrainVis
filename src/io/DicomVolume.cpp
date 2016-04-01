#include "DicomVolume.h"
#include <exception>

DicomVolume::DicomVolume(const std::string& path)
{
  if(!initData(path)){
      throw std::exception("could not init data");
  }
};
DicomVolume::DicomVolume(const std::string& rawFile,const std::string& metaFile){ };
DicomVolume::~DicomVolume(){ };

void DicomVolume::exportRawFile(const std::string& path){
  std::string filename = path+m_DicomParser.m_FileStacks[0]->m_strDesc+".raw";
  std::ofstream rawFile;
  rawFile.open (filename.c_str(), std::ios::out | std::ios::binary);
  rawFile.write ((char*)&m_vData[0], m_vData.size());
}

union char2short{
        char ch[2];
        short n;
    };


bool DicomVolume::initData(const std::string& DICOMpath){
  m_DicomParser.GetDirInfo(DICOMpath);

  //multiple files in folder, don't know which to take
  if(m_DicomParser.m_FileStacks.size() < 1){
  std::cout <<"[DICOMVolume] no correct dicom file "<<m_DicomParser.m_FileStacks.size()<<std::endl;
    return false;
  }

    //need better dimensions later !
    m_vDimensions.x = m_DicomParser.m_FileStacks[0]->m_ivSize.x;
    m_vDimensions.y = m_DicomParser.m_FileStacks[0]->m_ivSize.y;
    m_vDimensions.z = m_DicomParser.m_FileStacks[0]->m_Elements.size();

    m_vAspectRatio =  m_DicomParser.m_FileStacks[0]->m_fvfAspect;
    std::cout << "[DICOMVolume] dimensions"<< m_DicomParser.m_FileStacks[0]->m_fvfAspect << std::endl;

    Vec3f minPos(1000.0f,1000.0f,1000.0f);
    Vec3f maxPos(-1000.0f,-1000.0f,-1000.0f);
    for(int i = 0; i < m_DicomParser.m_FileStacks[0]->m_Elements.size();++i){
            SimpleDICOMFileInfo* dicomInfo = (SimpleDICOMFileInfo*)(m_DicomParser.m_FileStacks[0]->m_Elements[i]);
            if(dicomInfo->m_fvPatientPosition.x < minPos.x){ minPos.x = dicomInfo->m_fvPatientPosition.x; }
            if(dicomInfo->m_fvPatientPosition.y < minPos.y){ minPos.y = dicomInfo->m_fvPatientPosition.y; }
            if(dicomInfo->m_fvPatientPosition.z < minPos.z){ minPos.z = dicomInfo->m_fvPatientPosition.z; }

            if(dicomInfo->m_fvPatientPosition.x > maxPos.x){ maxPos.x = dicomInfo->m_fvPatientPosition.x; }
            if(dicomInfo->m_fvPatientPosition.y > maxPos.y){ maxPos.y = dicomInfo->m_fvPatientPosition.y; }
            if(dicomInfo->m_fvPatientPosition.z > maxPos.z){ maxPos.z = dicomInfo->m_fvPatientPosition.z; }
    }
    std::cout <<"[DICOMVolume] Patient minPos: "<< minPos << std::endl;
    std::cout <<"[DICOMVolume] Patient maxPos: "<< maxPos << std::endl;

    SimpleDICOMFileInfo* test = (SimpleDICOMFileInfo*)(m_DicomParser.m_FileStacks[0]->m_Elements[0]);
    //std::cout <<"[DICOMVolume] Patient Position: "<< test->m_fvPatientPosition << std::endl;
    std::cout <<"[DICOMVolume] Patient scale: "<< test->m_fScale << std::endl;
    std::cout <<"[DICOMVolume] Patient Bias: "<< test->m_fBias << std::endl;
    std::cout <<"[DICOMVolume] Patient windowwidth: "<< test->m_fWindowWidth << std::endl;
    std::cout <<"[DICOMVolume] Patient windowcenter: "<< test->m_fWindowCenter << std::endl;

    uint32_t voxelCount = m_vDimensions.x*m_vDimensions.y*m_vDimensions.z;
    uint32_t bytePerElement = m_DicomParser.m_FileStacks[0]->m_iAllocated/8;

    std::cout << voxelCount << " " << m_DicomParser.m_FileStacks[0]->m_ivSize << " " <<m_DicomParser.m_FileStacks[0]->m_Elements.size() << std::endl;
    std::cout <<"[DICOMVolume] byte per element: "<< bytePerElement << std::endl;

    //resize the vector to fit the complete volume
    //m_vData.resize(voxelCount*bytePerElement);
    m_vData.resize(voxelCount);

    //read each dicome slide and store them in the volume
    uint32_t dataOffset = 0;
    uint16_t* offsetPointer;
    for(int i = 0; i < m_DicomParser.m_FileStacks[0]->m_Elements.size();++i){
        dataOffset = i * (m_DicomParser.m_FileStacks[0]->m_ivSize.x *
                        m_DicomParser.m_FileStacks[0]->m_ivSize.y
                        );
        offsetPointer = &(m_vData[dataOffset]);
        m_DicomParser.m_FileStacks[0]->m_Elements[i]->GetData((char*)offsetPointer,m_DicomParser.m_FileStacks[0]->m_Elements[i]->GetDataSize(), 0);
    }
    //calculate the histogram;
    uint16_t currentValue = 0;
    uint16_t largestValue = 0;

    //find max value
    unsigned int sum = 0;
    for(int i = 0; i < m_vData.size();++i){
        currentValue = m_vData[i];
        //if(i < 100000)
            sum+= currentValue;

        if(largestValue < currentValue) largestValue = currentValue;
    }

    std::cout <<"[DICOMVolume] largest found value: "<< largestValue << std::endl;
    std::cout <<"[DICOMVolume] sum: "<< sum << std::endl;

    m_vHistogram.resize(largestValue+1);
    for(int i = 0; i < m_vData.size();++i){
        currentValue = m_vData[i];
        m_vHistogram[currentValue]++;
    }

    m_vHistogram.resize(largestValue);

    return true;
}

void DicomVolume::initData(const std::string& rawFile,const std::string& metaFile){};

void DicomVolume::exportJPGFiles(const std::string& path){
  std::vector<short>    shortData;
  std::string           filename;

  shortData.resize(m_vDimensions.x*m_vDimensions.y*m_vDimensions.z);
  memcpy(&shortData[0],&m_vData[0],m_vData.size());

  std::vector<char> final;
  final.resize(m_vDimensions.x*m_vDimensions.y);

  for(int i = 0; i < m_vDimensions.z;++i){

    int k = 0;
    for(int j = 0; j < m_vDimensions.x*m_vDimensions.y;j ++){
      final[k] = (uint8_t)(((float)shortData[i*(m_vDimensions.x*m_vDimensions.y)+j] / (float)m_vHistogram.size())*256.0f);
      k++;
    }

        filename = path+"output_"+std::to_string(i)+".jpg";

        write_JPEG_fileDICOMTEST((char*)filename.c_str(), 100, (uint8_t*) &(final[0]), m_vDimensions.x,m_vDimensions.y);
    }
}

int getIndex(int x, int y, int z, int dx, int dy, int dz){
    return  dx *  dy * z + dx *  y +x;
}

uint16_t DicomVolume::getValue(Core::Math::Vec3f volumePosition){
    if(volumePosition.x > 1.0f || volumePosition.x <0.0f ||
            volumePosition.y > 1.0f || volumePosition.y < 0.0f ||
            volumePosition.z > 1.0f || volumePosition.z < 0.0f ){
        return 0;
    }


  Core::Math::Vec3f arrayPos = volumePosition*Vec3f(m_vDimensions.x-1,m_vDimensions.y-1,m_vDimensions.z-1);
  arrayPos.x = std::max(0.0f,std::min( arrayPos.x,(float)(m_vDimensions.x-1)));
  arrayPos.y = std::max(0.0f,std::min( arrayPos.y,(float)(m_vDimensions.y-1)));
  arrayPos.z = std::max(0.0f,std::min( arrayPos.z,(float)(m_vDimensions.z-1)));

  Core::Math::Vec3f weights;
  weights.x = arrayPos.x - (int)arrayPos.x;
  weights.y = arrayPos.y - (int)arrayPos.y;
  weights.z = arrayPos.z - (int)arrayPos.z;


  Core::Math::Vec3i min = Core::Math::Vec3i(arrayPos.x-0.5f,arrayPos.y-0.5f,arrayPos.z-0.5f);
  Core::Math::Vec3i max = Core::Math::Vec3i(arrayPos.x+0.5f,arrayPos.y+0.5f,arrayPos.z+0.5f);

  min.x = std::max(0,std::min(min.x,(int32_t)(m_vDimensions.x-1)));
  min.y = std::max(0,std::min(min.y,(int32_t)(m_vDimensions.y-1)));
  min.z = std::max(0,std::min(min.z,(int32_t)(m_vDimensions.z-1)));

  max.x = std::max(0,std::min(min.x,(int32_t)(m_vDimensions.x-1)));
  max.y = std::max(0,std::min(min.y,(int32_t)(m_vDimensions.y-1)));
  max.z = std::max(0,std::min(min.z,(int32_t)(m_vDimensions.z-1)));

  float v1 = m_vData[getIndex(min.x,min.y,min.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v2 = m_vData[getIndex(min.x,min.y,max.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v3 = m_vData[getIndex(min.x,max.y,min.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v4 = m_vData[getIndex(min.x,max.y,max.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];

  float v5 = m_vData[getIndex(max.x,min.y,min.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v6 = m_vData[getIndex(max.x,min.y,max.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v7 = m_vData[getIndex(max.x,max.y,min.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];
  float v8 = m_vData[getIndex(max.x,max.y,max.z,m_vDimensions.x,m_vDimensions.y,m_vDimensions.z)];

  v1 = v1*(1.0f-weights.x) + v5*(weights.x);
  v2 = v2*(1.0f-weights.x) + v6*(weights.x);
  v3 = v3*(1.0f-weights.x) + v7*(weights.x);
  v4 = v4*(1.0f-weights.x) + v8*(weights.x);

  v1 = v1*(1.0f-weights.y) + v3*(weights.y);
  v2 = v2*(1.0f-weights.y) + v4*(weights.y);

  v1 = v1*(1.0f-weights.z) + v2*(weights.z);
  return v1;


  /*Core::Math::Vec3i arrayIndex = Core::Math::Vec3i(arrayPos.x+0.5f,arrayPos.y+0.5f,arrayPos.z+0.5f);
  int index = m_vDimensions.x*m_vDimensions.y*arrayIndex.z +
                m_vDimensions.x * arrayIndex.y +
                arrayIndex.x;
  uint16_t value = m_vData[index];

   std::cout <<"[DICOMVolume] array index"<< index<<std::endl;
  std::cout <<"[DICOMVolume] array float"<< arrayPos<<std::endl;
  std::cout <<"[DICOMVolume] array int"<< arrayIndex<<std::endl;
  std::cout <<"[DICOMVolume] array weights"<< weights<<std::endl;
  std::cout <<"[DICOMVolume] elements"<< m_vDimensions<<std::endl;
  std::cout << "[DicomVolume] VALUE: "<< value << std::endl;
  std::cout << "[DicomVolume] VALUEinterp: "<< v1 << std::endl;

  return value;*/

}

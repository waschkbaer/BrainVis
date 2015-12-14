#ifndef DICOMEVOLUME
#define DICOMEVOLUME

#include <vector>
#include <string>
#include <core/Math/Vectors.h>
#include <BrainVisIO/DICOM/DICOMParser.h>
#include <BrainVisIO/JPEGTool.h>
#include <core/Math/Vectors.h>
#include <algorithm>

#include <fstream>

class DicomVolume{
public:
  public:
  DicomVolume(std::string& path);
  DicomVolume(std::string& rawFile, std::string& metaFile);
  ~DicomVolume();

  void exportRawFile(std::string& path);
  void exportJPGFiles(std::string& path);

  std::vector<uint16_t>&      getData() {return m_vData; }
  std::vector<uint16_t>&  getHistogram() {return m_vHistogram;}

  Core::Math::Vec3ui   getDimensions() const {return m_vDimensions;}
  Core::Math::Vec3f    getAspectRatio() const {return m_vAspectRatio;}

protected:
  // initializes volume data from a path
  bool initData(std::string& DICOMpath);

  // loads a raw file which has been created from a dicom folder
  void initData(std::string& rawFile, std::string& metaFile);



private:
  std::vector<uint16_t> m_vData;
  std::vector<uint16_t> m_vHistogram;
  Core::Math::Vec3ui    m_vDimensions;
  Core::Math::Vec3f     m_vAspectRatio;

  DICOMParser           m_DicomParser;
};


#endif //DicomVolume

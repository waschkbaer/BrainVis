#ifndef DICOMEVOLUME
#define DICOMEVOLUME

#include <vector>
#include <string>
#include <core/Math/Vectors.h>
#include <io/DICOM/DICOMParser.h>
#include <io/JPEGTool.h>
#include <core/Math/Vectors.h>
#include <algorithm>

#include <fstream>

class DicomVolume{
public:
  public:
  DicomVolume(const std::string& path);
  DicomVolume(const std::string& rawFile,const std::string& metaFile);
  ~DicomVolume();

  void exportRawFile(const std::string& path);
  void exportJPGFiles(const std::string& path);

  const std::vector<uint16_t>&      getData() {return m_vData; }
  const std::vector<uint16_t>&  getHistogram() {return m_vHistogram;}

  Core::Math::Vec3ui   getDimensions() const {return m_vDimensions;}
  Core::Math::Vec3f    getAspectRatio() const {return m_vAspectRatio;}

  uint16_t             getValue(Core::Math::Vec3f volumePosition);

protected:
  // initializes volume data from a path
  bool initData(const std::string& DICOMpath);

  // loads a raw file which has been created from a dicom folder
  void initData(const std::string& rawFile,const std::string& metaFile);



private:
  std::vector<uint16_t> m_vData;
  std::vector<uint16_t> m_vHistogram;
  Core::Math::Vec3ui    m_vDimensions;
  Core::Math::Vec3f     m_vAspectRatio;

  DICOMParser           m_DicomParser;
  bool                  m_bCorrectData;
};


#endif //DicomVolume

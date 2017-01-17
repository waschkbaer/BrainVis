#include "io/MERNetworking/MERMeta.h"
#include "mocca/log/LogManager.h"

using namespace neuroio;

template<typename T>
T MERExtractor::readDataMER(const mocca::ByteArray& package, int& offset){
    T value;
    std::memcpy(&value,package.data()+offset*sizeof(char),sizeof(T));
    offset += sizeof(T);
    return value;
}

std::string MERExtractor::readStringMER(const mocca::ByteArray& package, int& offset, int length){
    char* c = new char[length];
    std::memcpy(c,package.data()+offset*sizeof(char),sizeof(char)*length);
    offset += length;
    std::string s(c,length);
    return s;
}

MERMeta MERExtractor::metaFromByteArray(const mocca::ByteArray& data){
    MERMeta meta;
    int offset = 4; //skip header!
    meta._frameNumber = readDataMER<int16_t>(data,offset);
    meta._protocoltype = readDataMER<int16_t>(data,offset);
    meta._triggerCorde = readDataMER<int32_t>(data,offset);
    meta._patientName = readStringMER(data,offset,50);
    meta._operationID = readDataMER<int32_t>(data,offset);
    meta._operationDate = readStringMER(data,offset,19);
    meta._currentTime = readStringMER(data,offset,19);
    meta._depth = readDataMER<int16_t>(data,offset);
    meta._stepsize = readDataMER<int16_t>(data,offset);

    ElectrodeConfiguration electrodeConfiguration;

    electrodeConfiguration._version = readStringMER(data,offset,40);

    MicroDriveInformation mdi;

    mdi._startingPosition = readDataMER<double>(data,offset);
    mdi._endingPosition = readDataMER<double>(data,offset);
    mdi._channel = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._microDrive = mdi;

    std::vector<ElectrodeInformation> vElectrodeInformation;

    for(int i = 0; i < 5;++i){
        ElectrodeInformation info;
        info._name = readStringMER(data,offset,40);
        info._channel = readDataMER<int32_t>(data,offset);
        vElectrodeInformation.push_back(info);
    }

    electrodeConfiguration._electrodeInformation = vElectrodeInformation;

    electrodeConfiguration._angle = readDataMER<int32_t>(data,offset);

    std::vector<DetailElecInformation> detailElec;

    for(int i = 0; i < 19;++i){
        DetailElecInformation info;
        info._active = readDataMER<int32_t>(data,offset);
        info._name = readStringMER(data,offset,40);
        info._upperFilterFrequency = readDataMER<double>(data,offset);
        info._lowerFilterFrequency = readDataMER<double>(data,offset);
        info._amplification = readDataMER<int32_t>(data,offset);
        info._channelColor = readDataMER<int32_t>(data,offset);
        info._upperSoftwareFilterFrequency = readDataMER<double>(data,offset);
        info._lowerSoftwareFilterFrequency = readDataMER<double>(data,offset);
        offset += 32;
        detailElec.push_back(info);
    }

    electrodeConfiguration._detailedEMGElectrodeInformation = detailElec;

    electrodeConfiguration._electrodeType = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._hemisphere = readDataMER<int32_t>(data,offset);

    std::vector<ElectrodeInformation> vElectrodeInformation2;

    for(int i = 0; i < 5;++i){
        ElectrodeInformation info;
        info._name = readStringMER(data,offset,40);
        info._channel = readDataMER<int32_t>(data,offset);
        vElectrodeInformation2.push_back(info);
    }

    electrodeConfiguration._electrodeSecondHeadbox = vElectrodeInformation2;

    MIElectrodeSettings MIES;

    MIES._upperFrequencyLimit = readDataMER<int32_t>(data,offset);
    MIES._lowerFrequencyLimit = readDataMER<int32_t>(data,offset);
    MIES._amplification = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._hardwareMERSettings = MIES;

    //skip IIR !
    offset += 20;

    electrodeConfiguration._borderEMG1 = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._borderEMG2 = readDataMER<int32_t>(data,offset);

    std::vector<TriggerChannelInfo> vTriggerInfo;

    for(int i = 0; i < 3; ++i){
        TriggerChannelInfo info;
        info._activeChannel = readDataMER<int32_t>(data,offset);
        info._name = readStringMER(data,offset,40);
        offset += 40;
        vTriggerInfo.push_back(info);
    }

    electrodeConfiguration._triggerChannelConfiguration = vTriggerInfo;

    electrodeConfiguration._LFPEMG1 = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._LFPEMG2 = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._connectorEMG1 = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._connectorEMG2 = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._measurmentTypeEMG1 = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._measurmentTypeEMG2 = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._IdEMG1 = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._IdEMG2 = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._MERLowerFrequencyLimit = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._MERUpperFrequencyLimit = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._STNLowerFrequencyLimit = readDataMER<int32_t>(data,offset);
    electrodeConfiguration._STNUpperFrequencyLimit = readDataMER<int32_t>(data,offset);

    electrodeConfiguration._ExtendedEMGRange[0] = readDataMER<bool>(data,offset);
    electrodeConfiguration._ExtendedEMGRange[1] = readDataMER<bool>(data,offset);
    electrodeConfiguration._ExtendedEMGRange[2] = readDataMER<bool>(data,offset);
    electrodeConfiguration._ExtendedEMGRange[3] = readDataMER<bool>(data,offset);

    offset += 6940;

    meta._electrodeConfiguration = electrodeConfiguration;

    meta._samplingRate = readDataMER<int16_t>(data,offset);
    meta._resolution = readDataMER<int8_t>(data,offset);

    meta._measuringActive = readDataMER<bool>(data,offset);
    meta._recordingActive = readDataMER<bool>(data,offset);
    meta._impedanceActive = readDataMER<bool>(data,offset);
    meta._stimulationActive = readDataMER<bool>(data,offset);

    meta._stimulationFrequency = readDataMER<int16_t>(data,offset);
    meta._stimulationPulseWidth = readDataMER<int16_t>(data,offset);
    meta._stimulationCurrent = readDataMER<int16_t>(data,offset);

    meta._CCactive = readDataMER<bool>(data,offset);

    meta._stimulatedSite = readDataMER<int16_t>(data,offset);

    std::vector<ImpedanceValues> vMERImpedance;

    for(int i = 0; i < 8;++i){
        ImpedanceValues values;
        values._positiveElectrode = readDataMER<int32_t>(data,offset);
        values._negativeElectrode = readDataMER<int32_t>(data,offset);
        vMERImpedance.push_back(values);
    }

    meta._MERImpedanceValues = vMERImpedance;

    std::vector<ImpedanceValues> vEMG1Impedance;

    for(int i = 0; i < 8;++i){
        ImpedanceValues values;
        values._positiveElectrode = readDataMER<int32_t>(data,offset);
        values._negativeElectrode = readDataMER<int32_t>(data,offset);
        vEMG1Impedance.push_back(values);
    }

    meta._EMG1ImpedanceValues = vEMG1Impedance;

    std::vector<ImpedanceValues> vEMG2Impedance;

    for(int i = 0; i < 8;++i){
        ImpedanceValues values;
        values._positiveElectrode = readDataMER<int32_t>(data,offset);
        values._negativeElectrode = readDataMER<int32_t>(data,offset);
        vEMG2Impedance.push_back(values);
    }

    meta._EMG2ImpedanceValues = vEMG2Impedance;

    return meta;
};

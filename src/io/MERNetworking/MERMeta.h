#pragma once

#include <vector>
#include <string>

#include "mocca/base/ByteArray.h"

namespace neuroio{

//Table10
struct ImpedanceValues {
    ImpedanceValues():
    _positiveElectrode(-1),
    _negativeElectrode(-1){};

    int32_t                             _positiveElectrode;
    int32_t                             _negativeElectrode;
};

//Table9
struct TriggerChannelInfo {
    TriggerChannelInfo():
    _activeChannel(-1),
    _name("") {};

    int32_t                             _activeChannel;
    std::string                         _name;
};


//Table8
struct IIRSettingInformation {
    IIRSettingInformation():
    _IRRFilterActive(false),
    _order(0.0),
    _frequency(0.0){};

    bool                                _IRRFilterActive;
    double                              _order;
    double                              _frequency;

};

//Table7
struct MIElectrodeSettings {
    MIElectrodeSettings():
    _upperFrequencyLimit(0),
    _lowerFrequencyLimit(0),
    _amplification(0) {};

    int32_t                             _upperFrequencyLimit;
    int32_t                             _lowerFrequencyLimit;
    int32_t                             _amplification;
};

//Table6
struct DetailElecInformation {
    DetailElecInformation():
    _active(0),
    _name(""),
    _upperFilterFrequency(0.0),
    _lowerFilterFrequency(0.0),
    _amplification(0),
    _channelColor(0),
    _upperSoftwareFilterFrequency(0.0),
    _lowerSoftwareFilterFrequency(0.0){};

    int32_t                             _active;
    std::string                         _name;
    double                              _upperFilterFrequency;
    double                              _lowerFilterFrequency;
    int32_t                             _amplification;
    int32_t                             _channelColor;
    double                              _upperSoftwareFilterFrequency;
    double                              _lowerSoftwareFilterFrequency;
};

//Table5
struct ElectrodeInformation {
    ElectrodeInformation():
    _name(""),
    _channel(0){};

    std::string                         _name;
    int32_t                             _channel;
};

//Table4
struct MicroDriveInformation {
    MicroDriveInformation():
    _startingPosition(0.0),
    _endingPosition(0.0),
    _channel(8) {};

    double                              _startingPosition;
    double                              _endingPosition;
    int32_t                             _channel;
};

//Table3
struct ElectrodeConfiguration {

    ElectrodeConfiguration():
    _version(""),
    _microDrive(),
    _electrodeInformation(),
    _angle(-1),
    _detailedEMGElectrodeInformation(),
    _electrodeType(-1),
    _hemisphere(-1),
    _electrodeSecondHeadbox(),
    _hardwareMERSettings(),
    _debugFilterSettings(),
    _borderEMG1(-1),
    _borderEMG2(-1),
    _triggerChannelConfiguration(),
    _LFPEMG1(-1),
    _LFPEMG2(-1),
    _connectorEMG1(-1),
    _connectorEMG2(-1),
    _measurmentTypeEMG1(-1),
    _measurmentTypeEMG2(-1),
    _IdEMG1(-1),
    _IdEMG2(-1),
    _MERLowerFrequencyLimit(-1),
    _MERUpperFrequencyLimit(-1),
    _STNLowerFrequencyLimit(-1),
    _STNUpperFrequencyLimit(-1),
    _ExtendedEMGRange(4){};

    std::string                         _version;
    MicroDriveInformation               _microDrive;// Table4
    std::vector<ElectrodeInformation>   _electrodeInformation; //Table5
    int32_t                             _angle;
    std::vector<DetailElecInformation>  _detailedEMGElectrodeInformation; //Table6
    int32_t                             _electrodeType;
    int32_t                             _hemisphere;
    std::vector<ElectrodeInformation>   _electrodeSecondHeadbox; //Table5
    MIElectrodeSettings                 _hardwareMERSettings; //Table7
    IIRSettingInformation               _debugFilterSettings; //Table8
    int32_t                             _borderEMG1;
    int32_t                             _borderEMG2;
    std::vector<TriggerChannelInfo>     _triggerChannelConfiguration; //Table9
    int32_t                             _LFPEMG1;
    int32_t                             _LFPEMG2;
    int32_t                             _connectorEMG1;
    int32_t                             _connectorEMG2;
    int32_t                             _measurmentTypeEMG1;
    int32_t                             _measurmentTypeEMG2;
    int32_t                             _IdEMG1;
    int32_t                             _IdEMG2;
    int32_t                             _MERLowerFrequencyLimit;
    int32_t                             _MERUpperFrequencyLimit;
    int32_t                             _STNLowerFrequencyLimit;
    int32_t                             _STNUpperFrequencyLimit;
    std::vector<bool>                   _ExtendedEMGRange;
};


struct MERMeta{

    MERMeta():
    _frameNumber(0),
    _protocoltype(0),
    _triggerCorde(0),
    _patientName(""),
    _operationID(-1),
    _operationDate(""),
    _currentTime(""),
    _depth(-1),
    _stepsize(-1),
    _electrodeConfiguration(),
    _samplingRate(0),
    _resolution(0),
    _measuringActive(true),
    _recordingActive(true),
    _impedanceActive(true),
    _stimulationActive(true),
    _stimulationFrequency(0),
    _stimulationPulseWidth(0),
    _stimulationCurrent(0),
    _CCactive(true),
    _stimulatedSite(0),
    _MERImpedanceValues(),
    _EMG1ImpedanceValues(),
    _EMG2ImpedanceValues() {};

    int16_t                             _frameNumber;
    int16_t                             _protocoltype;

    int32_t                             _triggerCorde;

    std::string                         _patientName;
    int32_t                             _operationID;
    std::string                         _operationDate;
    std::string                         _currentTime;
    int16_t                             _depth;
    int16_t                             _stepsize;

    ElectrodeConfiguration              _electrodeConfiguration;

    int16_t                             _samplingRate;
    int8_t                              _resolution;

    bool                                _measuringActive;
    bool                                _recordingActive;
    bool                                _impedanceActive;
    bool                                _stimulationActive;

    int16_t                             _stimulationFrequency;
    int16_t                             _stimulationPulseWidth;
    int16_t                             _stimulationCurrent;

    bool                                _CCactive;

    int16_t                             _stimulatedSite;

    std::vector<ImpedanceValues>        _MERImpedanceValues;   //Table10
    std::vector<ImpedanceValues>        _EMG1ImpedanceValues;  //Table10
    std::vector<ImpedanceValues>        _EMG2ImpedanceValues;  //Table10

};

class MERExtractor{
public:
    MERExtractor(){};
    ~MERExtractor(){};

    template<typename T>
    T readDataMER(const mocca::ByteArray& package, int& offset);

    std::string readStringMER(const mocca::ByteArray& package, int& offset, int length);


    MERMeta metaFromByteArray(const mocca::ByteArray& data);

};



};

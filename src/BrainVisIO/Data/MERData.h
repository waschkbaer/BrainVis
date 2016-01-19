#ifndef BRAINVIS_MERData
#define BRAINVIS_MERData

#include <vector>
#include <string>

namespace BrainVisIO{
namespace MERData{

class MERData{
public:
    MERData(int recordingDepth);
    MERData(int recordingDepth, const std::string& filename);
    virtual ~MERData(){};

    const std::vector<short>& getSignal(int seconds = 5);
    const std::vector<short>& getSignalFiltered(int seconds = 5, int lowFreq = 300, int highFreq = 5000);

    std::vector<double> getSpectralPower(int lowFreq = 200, int highFreq = 1000);
    std::vector<double> getSpectralPowerNormalized(int lowFreq = 200, int highFreq = 1000, int minVal = 2000, int maxVal = 25000);
    std::vector<double> getSpectralPowerNormalizedAndWindowed(int window = 10, int lowFreq = 200, int highFreq = 1000, int minVal = 1500, int maxVal = 25000);

    void addSignalRecording(short value);
    void addSignalRecording(std::vector<short>& values);

    void executeFFT(int seconds = 5, bool powerOfTwo = false);
    void executeFFTWelch(int seconds = 5, bool powerOfTwo = false);

    int getRecordedSeconds() const;

protected:
    void loadFile(const std::string& filename);

private:
    int                     _recordingDepth;
    int                     _recordedSeconds;
    std::vector<short>      _signal;
    std::vector<double>     _spectralPower;

    int                     _lastRequestTimer;
    std::vector<short>      _lastRequestedSeconds;
    std::vector<double>     _lastRequestedSpectralPower;

};

}
}


#endif

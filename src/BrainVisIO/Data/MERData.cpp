#include "MERData.h"

#include <BrainVisIO/FFT/fftwExecuter.h>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace BrainVisIO::MERData;

const int SAMPLESPERSECOND = 20000;

MERData::MERData(int recordingDepth):
_recordingDepth(recordingDepth),
_signal(),
_spectralPower(),
_lastRequestedSeconds(),
_lastRequestedSpectralPower(),
_lastRequestTimer(0),
_recordedSeconds(0){

}
MERData::MERData(int recordingDepth, const std::string& filename):
_recordingDepth(recordingDepth),
_signal(),
_spectralPower(),
_lastRequestedSeconds(),
_lastRequestedSpectralPower(),
_lastRequestTimer(0),
_recordedSeconds(0){

    loadFile(filename);
    executeFFTWelch(5,true);
}

const std::vector<short>& MERData::getSignal(int seconds){
    if(_signal.size() >= seconds*SAMPLESPERSECOND){
        _lastRequestedSeconds.resize(seconds*SAMPLESPERSECOND);
        _lastRequestTimer =  _signal.size();
        std::memcpy(&_lastRequestedSeconds[0],&_signal[_signal.size()-_lastRequestedSeconds.size()], _lastRequestedSeconds.size()* sizeof(short));
    }

    return _lastRequestedSeconds;
}
const std::vector<short>& MERData::getSignalFiltered(int seconds, int lowFreq, int highFreq){
    std::vector<short> signal = getSignal(seconds);
        if(signal.size() > 0){
            // DO HIGH PASS FILTER
            std::vector<short> signalFiltered;
            signalFiltered.resize(signal.size());

            float CUTOFF = lowFreq;
            float SAMPLE_RATE = SAMPLESPERSECOND;
            float RC = 1.0/(CUTOFF*2*3.14);
            float dt = 1.0/SAMPLE_RATE;
            float alpha = RC/(RC + dt);
                signalFiltered[0] = signal[0];
                for (int i = 1; i<signal.size(); i++){
                    signalFiltered[i] = alpha * (signalFiltered[i-1] + signal[i] - signal[i-1]);
                }
                signal.clear();
                signal.resize(0);
                signal = signalFiltered;


            // DO LOW PASS FILTER
           CUTOFF = highFreq;

            RC = 1.0/(CUTOFF*2*3.14);
            dt = 1.0/SAMPLE_RATE;
            alpha = dt/(RC+dt);
               signalFiltered[0] = signal[0];
              for(int i=1; i<signal.size(); i++){
                   signalFiltered[i] = signalFiltered[i-1] + (alpha*(signal[i] - signalFiltered[i-1]));
               }

           signal.clear();
           signal.resize(0);
           signal = signalFiltered;
            // DO LOW PASS FILTER END!
        }
        _lastRequestedSeconds = signal;
        return _lastRequestedSeconds;
}

std::vector<double> MERData::getSpectralPower(int lowFreq, int highFreq){
    std::vector<double> spectralData;

    if(_spectralPower.size() <= 0) return spectralData;

    int lowFreqIndex = (float)lowFreq/((float)SAMPLESPERSECOND/2.0f) * _spectralPower.size();
    int highFreqIndex = (float)highFreq/((float)SAMPLESPERSECOND/2.0f) * _spectralPower.size();
    int elementCount = highFreqIndex-lowFreqIndex;

    spectralData.resize(elementCount);

    std::memcpy(&spectralData[0],&_spectralPower[lowFreqIndex],elementCount*sizeof(double));

    return spectralData;
}
std::vector<double> MERData::getSpectralPowerNormalized(int lowFreq, int highFreq, int minVal, int maxVal){
    std::vector<double> spectralData = getSpectralPower(lowFreq,highFreq);
    if(spectralData.size() <= 0) return std::vector<double>();

    for(int i = 0; i < spectralData.size();++i){
        spectralData[i] = (spectralData[i]-minVal)/(maxVal-minVal);
        spectralData[i] = std::max(0.0,std::min(spectralData[i],1.0));
    }

    return spectralData;
}

std::vector<double> MERData::getSpectralPowerNormalizedAndWindowed(int window, int lowFreq, int highFreq, int minVal, int maxVal){
    std::vector<double> spectralData = getSpectralPower(lowFreq,highFreq);
    if(spectralData.size() <= 0) return std::vector<double>();

    double herzperindex = spectralData.size()/(float)(highFreq-lowFreq);

    std::vector<double> windowed;

    double val = 0;
    double counter = 0;
    for(float i = 0; i < spectralData.size();i += (window*herzperindex)){
        val = 0;
        counter = 0;
        for(float j = i; j < (i+(window*herzperindex)) && j < spectralData.size();++j){
            val += spectralData[j];
            counter++;
        }
        val = val/counter;
        val = (val-minVal)/(maxVal-minVal);
        val = std::max(0.0,std::min(val,1.0));
        windowed.push_back(val);
    }
    return windowed;
}

void MERData::addSignalRecording(short value){
    _signal.push_back(value);
    _recordedSeconds = _signal.size()/SAMPLESPERSECOND;
}
void MERData::addSignalRecording(std::vector<short>& values){
    for(const short s : values){
        _signal.push_back(s);
    }
    _recordedSeconds = _signal.size()/SAMPLESPERSECOND;
}

void MERData::executeFFT(int seconds, bool powerOfTwo){
    std::vector<short> signal = getSignalFiltered(seconds);
    std::vector<short> fftData;

    if(signal.size()/20000 >= seconds){
       int size = 1000*seconds/2*40;

       int arrSize = size;

       if(powerOfTwo){
          //calculate the exp
          int exp = 0;
          while(size > pow(2,exp)){
                    exp++;
          }
          arrSize = pow(2,exp-1);
          size = arrSize;

          fftData.resize(arrSize); //fft should use pow2 size

          memcpy( &(fftData[0]),
                  &(signal[0]),
                  size * sizeof(short));
       }else{
          fftData = signal;
       }

        FFTExecuter fftw;
        fftw.setInput(fftData);
        fftw.execute();
        fftw.calculateSpectralPower();
        _spectralPower = fftw.getSpectralPower();
    }

}
void MERData::executeFFTWelch(int seconds, bool powerOfTwo){
    std::vector<double> current;
        std::vector<double> fin;
        std::vector<double> input;

        std::vector<short> signal = getSignalFiltered(seconds);

        if(signal.size() > 0){
            int size = (float)signal.size()/4.5f; // 8 parts

            int arrSize = size;

            if(powerOfTwo){
                //calculate the exp
                int exp = 0;
                while(size > pow(2,exp)){
                    exp++;
                }
                arrSize = pow(2,exp-1);
                size = arrSize;
            }

            std::vector<short> fftData;
            fftData.resize(arrSize); //fft should use pow2 size
            input.resize(arrSize);

            for(int i = size-1; i < fftData.size();++i){
                fftData[i] = 0;
            }

            if(fin.size() != arrSize) fin.resize(arrSize);
            for(int i = 0; i < fin.size(); ++i){
                fin[i] = 0;
            }

            int halfSize = arrSize/2;
            for(int i = 0; i < 8; ++i){

                //copy window
                memcpy( &(fftData[0]),
                        &(signal[i*halfSize]),
                        arrSize * sizeof(short));

                //do hammy window function
                for(int k = 0; k < fftData.size();k++){
                    double d = 0.54f- (0.46f * cos( (2.0f* 3.14159265359f * (float)k) / (fftData.size()-1)  ));
                    input[k] = fftData[k]*d;
                }

                //fft of window
                FFTExecuter fftw;
                fftw.setInput(input);
                fftw.execute();
                fftw.calculateSpectralPower();

                current = fftw.getSpectralPower();

                if(fin.size() != current.size()) fin.resize(current.size());

                for(int j = 0; j < current.size();++j){
                    fin[j] += current[j]/8;
                }
            }

            _spectralPower = fin;
        }
}

void MERData::loadFile(const std::string& filename){
    std::string line;

    _signal.clear();
    _signal.resize(0);

    std::ifstream file (filename.c_str());
    if (file.is_open())
    {
        while ( std::getline (file,line) )
        {
            _signal.push_back((short)std::atoi(line.c_str()));
        }
    }

    _recordedSeconds = _signal.size()/SAMPLESPERSECOND;
}
int MERData::getRecordedSeconds() const
{
    return _recordedSeconds;
}


#include "fftwExecuter.h"

#include <math.h>

FFTExecuter::FFTExecuter(){

}

FFTExecuter::~FFTExecuter(){

}

void FFTExecuter::setInput(std::vector<unsigned short> realInput){
    _input.resize(realInput.size());
    for(int i = 0; i < realInput.size();++i){
        _input[i] = realInput[i];
    }
}

void FFTExecuter::setInput(std::vector<short> realInput){
    _input.resize(realInput.size());
    for(int i = 0; i < realInput.size();++i){
        _input[i] = realInput[i];
    }
}

 void FFTExecuter::setInput(std::vector<double> realInput){
    _input = realInput;
 }

void FFTExecuter::execute(){
    fftw_plan p;

    _output.resize(_input.size());
    p = fftw_plan_r2r_1d(_input.size(), &(_input[0]), &(_output[0]),FFTW_R2HC, FFTW_ESTIMATE);

    fftw_execute(p);
    fftw_destroy_plan(p);
}

void FFTExecuter::calculateSpectralPower(){
    _spectral.resize(_output.size()/2 + 1);

    _spectral[0] = sqrt(_spectral[0]*_spectral[0]);
    for (int k = 1; k < (_output.size()+1)/2; ++k)
        _spectral[k] = sqrt(_output[k]*_output[k] + _output[_output.size()-k]*_output[_output.size()-k]);
    if (_output.size() % 2 == 0)
        _spectral[_output.size()/2] = sqrt(_output[_output.size()/2]*_output[_output.size()/2]);


}

std::vector<double> FFTExecuter::getSpectralPower() const{
    return _spectral;
}

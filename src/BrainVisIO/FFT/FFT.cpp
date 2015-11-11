#include "FFT.h"

FFTCalc::FFTCalc():
    exp(1){

};
FFTCalc::~FFTCalc(){

};

std::vector<double> FFTCalc::getReal(){
    return _real;
}

std::vector<double> FFTCalc::getImaginary(){
    return _imaginary;
}

std::vector<double> FFTCalc::getSpectrum(){
    return _spectrum;
}

void FFTCalc::setInput(std::vector<double> d){
    _input = d;
}

void FFTCalc::execute(){
    exp = 1;
    while(pow(2,exp)< _input.size()){
        exp++;
    }

    _real.clear();
    _real.resize(pow(2,exp));
    _imaginary.clear();
    _imaginary.resize(pow(2,exp));

    for(int i = 0; i < pow(2,exp);i++){
        _real[0] = 0;
        _imaginary[0] = 0;
    }
    std::memcpy(&(_real[0]),&(_input[0]),_input.size()*sizeof(double));
    this->FFT(-1,exp,&(_real[0]),&(_imaginary[0]));
}

void FFTCalc::calculateSpectrum(){
    int N = _real.size();
    _spectrum.resize(N/2+1);

    _spectrum[0] = _real[0]*_real[0];
    for (int k = 1; k < (N+1)/2; ++k)
        _spectrum[k] = _real[k]*_real[k] + _real[N-k]*_real[N-k];
    if (N % 2 == 0)
        _spectrum[N/2] = _real[N/2]*_real[N/2];
}

void FFTCalc::calculateSpectrum2(){
    int N = _real.size();
    _spectrum.resize(N/2+1);

    _spectrum[0] = std::sqrt((_real[0]*_real[0]) + (_imaginary[0]*_imaginary[0]));
    for (int k = 1; k < (N+1)/2; ++k)
        _spectrum[k] =  std::sqrt((_real[k]*_real[k]) + (_imaginary[k]*_imaginary[k]));
    if (N % 2 == 0)
        _spectrum[N/2] = std::sqrt((_real[N/2]*_real[N/2]) + (_imaginary[N/2]*_imaginary[N/2]));

    /*_spectrum[0] = std::sqrt((_real[0]*_real[0]) + (_imaginary[0]*_imaginary[0]));
        for (int k = 1; k < (N+1)/2; ++k)
            _spectrum[k] =  std::sqrt((_real[k]*_real[k]) + (_imaginary[k]*_imaginary[k]))     +
                            std::sqrt((_real[N-k]*_real[N-k]) + (_imaginary[N-k]*_imaginary[N-k]));
        if (N % 2 == 0)
            _spectrum[N/2] = std::sqrt((_real[N/2]*_real[N/2]) + (_imaginary[N/2]*_imaginary[N/2]));*/
}


void FFTCalc::FFT(short int dir,long m,double *x,double *y)
{
    long n,i,i1,j,k,i2,l,l1,l2;
    double c1,c2,tx,ty,t1,t2,u1,u2,z;

    /* Calculate the number of points */
    n = 1;
    for (i=0;i<m;i++)
        n *= 2;

    /* Do the bit reversal */
    i2 = n >> 1;
    j = 0;
    for (i=0;i<n-1;i++) {
        if (i < j) {
            tx = x[i];
            ty = y[i];
            x[i] = x[j];
            y[i] = y[j];
            x[j] = tx;
            y[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    /* Compute the FFT */
    c1 = -1.0;
    c2 = 0.0;
    l2 = 1;
    for (l=0;l<m;l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0;
        u2 = 0.0;
        for (j=0;j<l1;j++) {
            for (i=j;i<n;i+=l2) {
                i1 = i + l1;
                t1 = u1 * x[i1] - u2 * y[i1];
                t2 = u1 * y[i1] + u2 * x[i1];
                x[i1] = x[i] - t1;
                y[i1] = y[i] - t2;
                x[i] += t1;
                y[i] += t2;
            }
            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (dir == 1)
            c2 = -c2;
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    /* Scaling for forward transform */
    if (dir == 1) {
        for (i=0;i<n;i++) {
            x[i] /= n;
            y[i] /= n;
        }
    }
}

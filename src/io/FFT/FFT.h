#ifndef FFTBAULBOURKE
#define FFTBAULBOURKE

#include <vector>
#include <math.h>

class FFTCalc{
public:
    FFTCalc();
    ~FFTCalc();

    std::vector<double> getReal();

    std::vector<double> getImaginary();

    std::vector<double> getSpectrum();

    void setInput(std::vector<double> d);

    void execute();

    void calculateSpectrum();

    void calculateSpectrum2();

private:
    void FFT(short int dir,long m,double *x,double *y);


private:
    std::vector<double> _real;
    std::vector<double> _imaginary ;
    std::vector<double> _input ;
    std::vector<double> _spectrum;
    int exp;
};
#endif // FFTBAULBOURKE


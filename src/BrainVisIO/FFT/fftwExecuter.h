#ifndef FFTWEXECUTER
#define FFTWEXECUTER

#include "../../FFTWOSX/fftw3.h"

#include <vector>

class FFTExecuter{
public:
    FFTExecuter();
    ~FFTExecuter();

    void setInput(std::vector<unsigned short> realInput);
    void setInput(std::vector<short> realInput);
    void setInput(std::vector<double> realInput);

    void execute();

    void calculateSpectralPower();

    std::vector<double> getSpectralPower() const;
private:

    std::vector<double> _input;
    std::vector<double> _output;
    std::vector<double> _spectral;
};

#endif

#ifndef PERCEPTRON
#define PERCEPTRON

#include <vector>

class Perceptron{
public:
    Perceptron();
    ~Perceptron();


    std::vector<double> weights() const;
    void setWeights(const std::vector<double> &weights);

    std::vector<double> inputs() const;
    void setInputs(const std::vector<double> &inputs);

    void setBaseWeights(int n);
    double execute(double k);
    bool train(bool expected);
    bool classify();

    double threshold() const;
    void setThreshold(double threshold);

private:
    std::vector<double> _weights;
    std::vector<double> _inputs;
    double              _erg;
    double              _threshold;
    double              _learnrate;
};

#endif

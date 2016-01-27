#include "Perceptron.h"
#include <iostream>

Perceptron::Perceptron():
_threshold(0),
_learnrate(0.5){

}

Perceptron::~Perceptron(){

}

std::vector<double> Perceptron::weights() const
{
    return _weights;
}

void Perceptron::setWeights(const std::vector<double> &weights)
{
    _weights = weights;
}

std::vector<double> Perceptron::inputs() const
{
    return _inputs;
}

void Perceptron::setInputs(const std::vector<double> &inputs)
{
    _inputs = inputs;
}

double Perceptron::execute(double k){

    double erg = 0;

    for(int i = 0; i < _weights.size();i++){
        erg += _weights[i]*_inputs[i];
    }

    _erg = erg - k;
    return _erg;
}

bool Perceptron::train(bool expected){
    execute(_threshold);

    if(_learnrate > 0.01)
        _learnrate -= 0.002f;

    if( (_erg >= 0 && expected) || (_erg < 0 && !expected) ){
          //std::cout << "Richtig klassifiziert " << _erg<<std::endl;
          return true;
    }else{
         // std::cout << "ERWARTET:" << expected << " Erhalten: " << _erg << std::endl;

          if(expected && _erg < 0){
              if(_threshold +_erg < _threshold && _threshold +_erg >= 0)
                    _threshold = _threshold+_erg;

                for(int i = 0; i < _weights.size();++i){
                    _weights[i] = _weights[i] + _learnrate*_inputs[i];
                }

          }

          if(!expected && _erg >= 0){
              for(int i = 0; i < _weights.size();++i){
                  _weights[i] = _weights[i] - _learnrate* _inputs[i];
              }
          }
          return false;
    }
}

bool Perceptron::classify(){
    execute(_threshold);
    if(_erg >= 0 ){
          return true;
    }else{
          return false;
    }
}


double Perceptron::threshold() const
{
    return _threshold;
}

void Perceptron::setThreshold(double threshold)
{
    _threshold = threshold;
}


void Perceptron::setBaseWeights(int n){
    _weights.resize(n);
    for(int i = 0; i < n;i++){
        _weights[i] = 1;
    }
}

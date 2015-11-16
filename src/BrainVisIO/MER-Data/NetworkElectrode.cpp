#include "NetworkElectrode.h"
#include <fstream>
#include <sstream>
#include <string>
#include <core/splittools.h>
#include <algorithm>

#include "NetworkMERData.h"

NetworkElectrode::NetworkElectrode(std::string name):
    iElectrode(name){
}
NetworkElectrode::~NetworkElectrode(){


}

void NetworkElectrode::updateElectrode(){
    //this should be some network data! change later
    std::vector<double> input;
    Core::Math::Vec3f  position(100,100,100);
    int8_t depth = 0;


    //create the data
    std::shared_ptr<NetworkMERData> data = std::make_shared<NetworkMERData>();

    //setinput
    data->setDataPosition(position);
    data->setFrequency(2000);
    data->setFrequencyRange(_analysisRange);
    data->setInput(input); //will execute the fft function

    data->executeFFT();

    addData(depth,data);
}

void NetworkElectrode::recalculateFrequencyRange(){
    for(int i = (int8_t)_depthRange.x; i <= _depthRange.y; ++i){
        if(_electrodeData.find(i) != _electrodeData.end()){
           _electrodeData.find(i)->second->executeFFT();
        }
    }
}

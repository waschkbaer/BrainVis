#include "NetworkElectrode.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "NetworkMERData.h"

NetworkElectrode::NetworkElectrode(std::string name):
    iElectrode(name){
}
NetworkElectrode::~NetworkElectrode(){


}

void NetworkElectrode::updateElectrode(){
    //request idea
    /*
     * int d = MERconnection->getCurDepth(this->name)
     * if(d > depthRange.y){
     *      position = MERConnection->getPosition(this->name,d);
     *      signal = MERConnection->getSignla(this->name,d);
     *
     *      update/add etc
     *      for(driveWidget w: driveWidgetList){
     *          w->update()
     *      }
     *      for(renderer r: rendererList){ //not needed for renderers ! just call shedule redraw! will do the rest i guess!
     *          r->update();
     *      }
     *
     * }else{
     *  just do nothing and request again in some future!
     * }
     *
     *
     * */

    //this should be some network data! change later
    std::vector<double> input;
    Core::Math::Vec3f  position(100,100,100);
    int8_t depth = 0;
    double frequency = 2000.0;


    //create the data
    std::shared_ptr<NetworkMERData> data = std::make_shared<NetworkMERData>();

    //setinput
    data->setDataPosition(position);
    data->setFrequency(frequency);
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

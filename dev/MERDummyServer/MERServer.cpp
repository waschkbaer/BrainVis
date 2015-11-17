#include "MERServer.h"
#include <cstdlib>

MERServer::MERServer(){
    srand (time(NULL));
}
MERServer::~MERServer(){

}

void MERServer::run(){

}

void MERServer::updateData(std::string electrodeName){

}

void MERServer::generateData(){

}

void MERServer::sendData(){

}

void MERServer::listData(){

}


void MERServer::initFakeData(){

    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("LLat",new std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("LCen",new std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("LAnt",new std::vector<Data>()));

    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("RLat",new std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("RCen",new std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>*> ("RAnt",new std::vector<Data>()));

    Data f1;
    int randomNumb;
    f1._depth = -10;

    for(auto it = _electrodes.begin(); it != _electrodes.end();++it){


        for(int i = 0; i < 400;++i){
            randomNumb = rand()% 1000;
            randomNumb -= 500;
            f1._signal.push_back((double)randomNumb/100.0);
        }
    }



}

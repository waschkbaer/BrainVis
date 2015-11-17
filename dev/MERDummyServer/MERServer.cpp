#include "MERServer.h"
#include <cstdlib>
#include <iostream>

MERServer::MERServer(){
    srand (time(NULL));
    initFakeData();
}
MERServer::~MERServer(){

}

void MERServer::run(){
      int i = 0;
      std::cout << "wait for input !"<< std::endl;
      std::cin >> i;
      if(i == 1){
          std::cout << "will do left step to next"<< std::endl;
          generateDataStepLeft();
          printInformation();
      }
      if(i == 2){
          std::cout << "will do right step to next"<< std::endl;
          generateDataStepRight();
          printInformation();
      }

}

void MERServer::generateDataStepLeft(){
     for(auto it = _electrodes.begin(); it != _electrodes.end();++it){
         if(it->first.find("LLat") != std::string::npos ||
            it->first.find("LAnt") != std::string::npos ||
            it->first.find("LCen") != std::string::npos ){

             int randomNumb = 0;
             int curDepth = it->second[it->second.size()-1]._depth;
             curDepth++;
             if(curDepth <= 4){
                 Data f1;
                 f1._depth = curDepth;

                 if(it->first.find("LLat") != std::string::npos){
                     f1._position = LLatPos[curDepth+10];
                 }else if(it->first.find("LAnt") != std::string::npos){
                     f1._position = LAntPos[curDepth+10];
                 }else if(it->first.find("LCen") != std::string::npos){
                     f1._position = LCenPos[curDepth+10];
                 }


                 for(int i = 0; i < 400;++i){
                     randomNumb = rand()% 1000;
                     randomNumb -= 500;
                     f1._signal.push_back((double)randomNumb/100.0);
                 }
                 it->second.push_back(f1);
             }
         }
     }
}

void MERServer::generateDataStepRight(){
    for(auto it = _electrodes.begin(); it != _electrodes.end();++it){
        if(it->first.find("RLat") != std::string::npos ||
           it->first.find("RAnt") != std::string::npos ||
           it->first.find("RCen") != std::string::npos ){

            int randomNumb = 0;
            int curDepth = it->second[it->second.size()-1]._depth;
            curDepth++;
            if(curDepth <= 4){
                Data f1;
                f1._depth = curDepth;

                if(it->first.find("RLat") != std::string::npos){
                    f1._position = RLatPos[curDepth+10];
                }else if(it->first.find("RAnt") != std::string::npos){
                    f1._position = RAntPos[curDepth+10];
                }else if(it->first.find("RCen") != std::string::npos){
                    f1._position = RCenPos[curDepth+10];
                }


                for(int i = 0; i < 400;++i){
                    randomNumb = rand()% 1000;
                    randomNumb -= 500;
                    f1._signal.push_back((double)randomNumb/100.0);
                }
                it->second.push_back(f1);
            }
        }
    }
}

void MERServer::printInformation(){
    for(auto it = _electrodes.begin(); it != _electrodes.end();++it){
        std::cout << "current electrode: < "<< it->first << " > storeded datapoints: "<< it->second.size()<<std::endl;

        for(int i = 0; i < it->second.size();++i){
            Data d = it->second[i];
            std::cout << "depth: "<< std::to_string(d._depth) << " position: "<< d._position << " signalsize: "<< d._signal.size()<<std::endl;
        }
    }
}

void MERServer::sendData(){

}

void MERServer::listData(){

}


void MERServer::initFakeData(){

    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("LLat",std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("LCen",std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("LAnt",std::vector<Data>()));

    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("RLat",std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("RCen",std::vector<Data>()));
    _electrodes.insert(std::pair<std::string,std::vector<Data>> ("RAnt",std::vector<Data>()));

    int randomNumb;
    for(auto it = _electrodes.begin(); it != _electrodes.end();++it){
         Data f1;
         f1._depth = -10;

         if(it->first.find("LLat") != std::string::npos){
             f1._position = LLatPos[0];
         }else if(it->first.find("LAnt") != std::string::npos){
             f1._position = LAntPos[0];
         }else if(it->first.find("LCen") != std::string::npos){
             f1._position = LCenPos[0];
         }else if(it->first.find("RLat") != std::string::npos){
             f1._position = RLatPos[0];
         }else if(it->first.find("RAnt") != std::string::npos){
             f1._position = RAntPos[0];
         }else if(it->first.find("RCen") != std::string::npos){
             f1._position = RCenPos[0];
         }



        for(int i = 0; i < 400;++i){
            randomNumb = rand()% 1000;
            randomNumb -= 500;
            f1._signal.push_back((double)randomNumb/100.0);
        }

        it->second.push_back(f1);
    }



}


void MERServer::getRegisteredElectrodes(){}
void MERServer::getLatestDepth(std::string name){}
void MERServer::getNextDepth(std::string name){}

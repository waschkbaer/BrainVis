#include "MERServer.h"
#include <cstdlib>
#include <iostream>

#include "logging/logmanager.h"
#include "logging/consolelog.h"
#include "logging/htmllog.h"
#include "logging/textlog.h"
using namespace ghoul::logging;

std::vector<std::string> &splitStringDump(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> splitStringDump(const std::string &s, char delim) {
    std::vector<std::string> elems;
    splitStringDump(s, delim, elems);
    return elems;
}

MERServer::MERServer(int port){
    srand (time(NULL));
    initFakeData();

    TCPNetworkService netService;
    _listener  = netService.bind(std::to_string(port)); // listen for connections
}
MERServer::~MERServer(){

}

void MERServer::run(){
      int i = 0;
      LINFOC("MERDUMMYSERVER","waiting for input");
      std::cin >> i;
      if(i == 1){
          LINFOC("MERDUMMYSERVER","will do left step to next");
          generateDataStepLeft();
          printInformation();
      }
      if(i == 2){
          LINFOC("MERDUMMYSERVER","will do right step to next");
          generateDataStepRight();
          printInformation();
      }

}

void MERServer::startNetworkThread(){
    _networkThread = std::unique_ptr<std::thread>(new std::thread(&MERServer::networkerRun,this));
}

void MERServer::networkerRun(){
        bool keepAlive = true;
        LINFOC("MERDUMMYSERVER","waiting for connection");
        while(_connection == nullptr){
            _connection = _listener->getConnection();
        }
        LINFOC("MERDUMMYSERVER","got a connection");

        while(keepAlive){
            try{
                BytePacket data = _connection->receive();
                if(data.byteArray().size() > 0){
                    std::string s(data.byteArray().data(),0,data.byteArray().size());
                    handleMsg(s);
                }
            }catch(const ConnectionClosedError& err){
                LERRORC("MERDUMMYSERVER", "lost connection");
                _connection == nullptr;
                keepAlive = false;
            }catch(const NetworkError& err){
                keepAlive = false;
                _connection == nullptr;
                LERRORC("MERDUMMYSERVER", "network error");
            }
        }
}

void MERServer::handleMsg(std::string s){
    LINFOC("MERDUMMYSERVER", "MSG: "+s);

    std::vector<std::string> msgPara = splitStringDump(s,':');

    if(msgPara[0] == "GETELC"){
        std::string answer;

        for(auto it = _electrodes.begin(); it != _electrodes.end();++it){
            answer += it->first;
            answer += ":";
        }
        answer = answer.substr(0,answer.size()-1);
        LINFOC("MERDUMMYSERVER","ANSWER: "+ answer);

        ByteArray stringArray;
        stringArray.append(answer.c_str(),answer.size());

        _connection->send(stringArray);
    }else if(msgPara[0] == "POS"){
        std::vector<Data> dat = _electrodes.find(msgPara[1])->second;
        Core::Math::Vec3f pos = dat[std::atoi(msgPara[2].c_str())+10]._position;

        LINFOC("MERDUMMYSERVER","ANSWER: "<< pos.x << "," << pos.y << "," << pos.z);
        ByteArray depthArr;
        depthArr.append(&pos.x,sizeof(float));
        depthArr.append(&pos.y,sizeof(float));
        depthArr.append(&pos.z,sizeof(float));

        _connection->send(depthArr);
    }else if(msgPara[0] == "CDEPTH"){
        std::vector<Data> dat = _electrodes.find(msgPara[1])->second;
        int32_t depth = dat[dat.size()-1]._depth;

        ByteArray depthArr;
        depthArr.append(&depth,sizeof(int32_t));

        _connection->send(depthArr);
    }else if(msgPara[0] == "SIGNAL"){
        std::vector<Data> dat = _electrodes.find(msgPara[1])->second;
        std::vector<double> signal = dat[dat.size()-1]._signal;

        ByteArray signalArr;
        signalArr.append(&(signal[0]),sizeof(double)*signal.size());

        _connection->send(signalArr);
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

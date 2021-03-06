#include "MERClient.h"



#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <math.h>
#include <thread>


#include <io/Data/MERElectrode.h>
#include <io/Data/MERData.h>

using namespace mocca;
using namespace mocca::net;
using namespace std;


//max pack size = 400000

static int NUM_CHAN = 8;
static int NUM_SAMP_PER_CHAN = 40;
static int NUM_BYTES_PER_CHAN = NUM_SAMP_PER_CHAN*2;
static int NUM_BYTES_DATA = NUM_BYTES_PER_CHAN*NUM_CHAN;

static int NUM_BYTES_HEADER = 60;
static int NUM_BYTES_FOOTER = 4;

//EACH Payload is 2 ms
static int NUM_PAYLOADS_READ = 250;
//the number of bytes in each payload
static int NUM_BYTES_PAYLOAD = NUM_CHAN * NUM_BYTES_PER_CHAN + NUM_BYTES_HEADER + NUM_BYTES_FOOTER;

//How many bytes should be read at once
static int TOTAL_BYTES_READ = NUM_BYTES_PAYLOAD*NUM_PAYLOADS_READ;

static int PAYLOAD_COUNT = 0;


MERClient::MERClient(int maxpackagesize):
_isConnected(false),
_MAXPACKAGESIZE(maxpackagesize),
  _isRecording(false),
  _currentDepth(11)
{
    //ConnectionFactorySelector::addDefaultFactories();
}

MERClient::~MERClient(){

}

void MERClient::connect(std::string hostname, int port, MERElectrodeIds electrodeSettings){
    std::string address = hostname+":"+std::to_string(port);
    connect(address,electrodeSettings);

}

void MERClient::connect(std::string hostnamePort, MERElectrodeIds electrodeSettings){
    _electrodeSettings = electrodeSettings;
    try {
        cout << "[MERClient] will connect to  : " << hostnamePort  << endl;
        _connection = ConnectionFactorySelector::streamConnectionFactory("tcp").connect(hostnamePort);

        _isConnected = true;
        cout << "[MERClient] connection established" << endl;

        _clientThread = std::unique_ptr<std::thread>(new std::thread(&MERClient::run,this));
         cout << "[MERClient] connection thread started" << endl;
    }catch(const ConnectFailedError& err){
        std::cout <<"[MERClient] "<< err.what()<<std::endl;
        _isConnected = false;
    }catch(const NetworkError& err){
        std::cout <<"[MERClient] "<< err.what()<<std::endl;
        _isConnected = false;
    }
}

bool MERClient::read(){
    std::string output = "[MERClient] ";
    if(_isConnected){
        merClientMutex.lock();
        std::shared_ptr<BrainVisIO::MERData::MERData> cenData = _currentBundle->
                getElectrode("cen")->
                getMERData(_currentDepth);

        std::shared_ptr<BrainVisIO::MERData::MERData> latData = _currentBundle->
                getElectrode("lat")->
                getMERData(_currentDepth);

        std::shared_ptr<BrainVisIO::MERData::MERData> antData = _currentBundle->
                getElectrode("ant")->
                getMERData(_currentDepth);
        merClientMutex.unlock();

        try{
            //read some data smaller / equal to macpackagesize
            ByteArray input = _connection->receive(200000);

            //sanity check : any data ?
            if(input.size() <= 0){
                //cout << "[MERClient] No data"<< endl;
                return false;
            }else{
                //cout << "[MERClient] "<< input.size()<< endl;
            }

            output += "Packetsize["+std::to_string(input.size())+"] ";

            //copy the complete read data into a short vector
            //maybe needs some endianess check
            std::vector<uint16_t> data;
            data.resize(TOTAL_BYTES_READ/2);
            memcpy(&(data[0]),input.data(),TOTAL_BYTES_READ);

            int nextIndex = _data.size();
            _data.resize(_data.size()+(input.size()));
            memcpy(&(_data[nextIndex]),input.data(),input.size());
            //std::cout << "data size: "<< _data.size() << std::endl;

            //find header index and push them to vector // there should be 250 headers!
            std::vector<uint32_t> headerIndex;
            for(int i = 0; i < _data.size();++i){
                if(_data[i] == 32896 && _data[i + 1] == 32639 && _data[i + 3] == 256) // 32896 dec = 8080 hex
                {
                    headerIndex.push_back(i);
                }
            }

            //std::cout << "header found: "<< headerIndex.size()<<std::endl;

            int num_cycle = 0; //what cycle??
            int dataStart = 0;
            int dataEnd  = 0;
            int DataOffset = 0;

            std::vector<uint16_t> temp;
            temp.resize(NUM_BYTES_DATA/2);

            //iterate over all headerIndices
            for(int i = 0; i < headerIndex.size();++i){
                PAYLOAD_COUNT++;

                //increment cycle
                num_cycle++;

                dataStart = headerIndex[i]+(NUM_BYTES_HEADER/2); // data starts 30 short later (header)
                dataEnd = dataStart + (NUM_BYTES_DATA/2) - 1;

                if(dataEnd > _data.size()-1){
                    //std::cout << "[MERClient] end of dataarray" << std::endl;
                    continue;
                }

                //memcpy of current data
                memcpy(&(temp[0]),&(_data[dataStart]),NUM_BYTES_DATA);

                DataOffset = NUM_BYTES_PER_CHAN/2;

                                int32_t value = 0;
                //read some channel data
                for(int j = 0; j < temp.size();j = j+8){
                    if(_electrodeSettings._cen != -1 && cenData != nullptr){
                        merClientMutex.lock();
                        value = temp[j+_electrodeSettings._cen];
                        value -= (int)(65535.0f/2.0f);
                        cenData->addSignalRecording(value);
                        merClientMutex.unlock();
                    }

                    if(_electrodeSettings._lat != -1){
                        merClientMutex.lock();
                        value = temp[j+_electrodeSettings._lat];
                        value -= (int)(65535.0f/2.0f);
                        latData->addSignalRecording(value);
                        merClientMutex.unlock();
                    }

                    if(_electrodeSettings._ant != -1){
                        merClientMutex.lock();
                        value = temp[j+_electrodeSettings._ant];
                        value -= (int)(65535.0f/2.0f);
                        antData->addSignalRecording(value);
                        merClientMutex.unlock();
                    }
                }
            }

            for(int i = headerIndex.size()-1; i >= 0 ;i--){
                dataStart = headerIndex[i];
                dataEnd = headerIndex[i]+ NUM_BYTES_PAYLOAD;

                if(dataEnd > _data.size())
                    continue;


                int missing = _data.size()-dataEnd;

                std::vector<uint16_t> dataTemp;
                dataTemp.resize(_data.size() - NUM_BYTES_PAYLOAD);
                memcpy(&(dataTemp[0]),&(_data[0]),dataStart);
                memcpy(&(dataTemp[dataStart+1]),&(_data[dataEnd]),missing);
                _data = dataTemp;


            }

            //std::cout << "_data size after loops"<< _data.size() << std::endl;

            //if(num_cycle != 250)
                //std::cout << output << std::endl;

        }catch(const ConnectionClosedError& err){
            std::cout <<"[MERClient] "<< err.what()<<std::endl;
            _isConnected = false;
        }catch(const NetworkError& err){
            std::cout <<"[MERClient] "<< err.what()<<std::endl;
            _isConnected = false;
        }
        return true;
    }else{
        return false;
    }
}

bool MERClient::flush(){
    if(_isConnected){

        try{
            //read stuff while not "recoding"
            ByteArray input = _connection->receive(_MAXPACKAGESIZE);

        }catch(const ConnectionClosedError& err){
            std::cout <<"[MERClient] "<< err.what()<<std::endl;
            _isConnected = false;
        }catch(const NetworkError& err){
            std::cout <<"[MERClient] "<< err.what()<<std::endl;
            _isConnected = false;
        }
        return true;
    }else{
        return false;
    }
}

void MERClient::resetMERClient(){
    PAYLOAD_COUNT = 0;

    _currentDepth--;
}

bool MERClient::isConnected(){
    return _isConnected;
}

int MERClient::getCurrentDepth() const
{
    return _currentDepth;
}

void MERClient::setCurrentDepth(int currentDepth)
{
    _currentDepth = currentDepth;
}
std::shared_ptr<BrainVisIO::MERData::MERBundle> MERClient::getCurrentBundle() const
{
    return _currentBundle;
}

void MERClient::setCurrentBundle(const std::shared_ptr<BrainVisIO::MERData::MERBundle> &currentBundle)
{
    _currentBundle = currentBundle;
}
bool MERClient::getIsRecording() const
{
    return _isRecording;
}

void MERClient::setIsRecording(bool isRecording)
{
    _isRecording = isRecording;
}
void MERClient::setIsConnected(bool isConnected)
{
    _isConnected = isConnected;
}

void MERClient::waitForThread(){
    _clientThread->join();
}

void MERClient::run(){
    while(_isConnected){
        std::this_thread::sleep_for(std::chrono::microseconds(150));
        if(_isRecording){
            read();
        }else{
            flush();
        }
    }
    std::cout << "[MERClient] thread ends" <<std::endl;
}

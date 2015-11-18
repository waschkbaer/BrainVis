#include "MERConnection.h"
#include <algorithm>

#include <net/Error.h>

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

void MERConnection::openConnection(std::string hostname, int port){
    if(!_isConnected){
        try{
            TCPNetworkService netService;
            std::string s = hostname + ":"+ std::to_string(port);
            std::cout << "Will try to connect to MER device at : "<<s <<std::endl;
            _connection = netService.connect(s);
            _isConnected = true;
        }catch (const ConnectionClosedError& err){
          //LERRORC("NetworkRendererClient", "lost connection");
            _isConnected = false;
        }
        catch (const NetworkError& err){
          //LERRORC("NetworkRendererClient", "network error");
            _isConnected = false;
        }
    }
}

void MERConnection::closeConnection(){
    _isConnected = false;
}

std::vector<std::string> MERConnection::getRegisteredElectrodes(){
    std::vector<std::string> electrodeList;

    if(!_isConnected) return electrodeList;

    try{
         _connectionMutex.lock();
        sendString("GETELC");

        BytePacket data = _connection->receive();
        if(data.byteArray().size() > 0){
            std::string s(data.byteArray().data(),0,data.byteArray().size());
            electrodeList = splitStringDump(s,':');
        }
      }
      catch (const ConnectionClosedError& err){
        //LERRORC("NetworkRendererClient", "lost connection");
      }
      catch (const NetworkError& err){
        //LERRORC("NetworkRendererClient", "network error");
      }
     _connectionMutex.unlock();
    return electrodeList;
}

int MERConnection::getCurrentDepth(std::string electrodeIdentifier){
    int32_t depth = -20;
    if(!_isConnected) return depth;

    std::string request = "CDEPTH:"+electrodeIdentifier;

    try{
        _connectionMutex.lock();
        sendString(request);

        BytePacket data = _connection->receive();
        if(data.byteArray().size() > 0){
            depth = data.get<int32_t>();
        }
      }
      catch (const ConnectionClosedError& err){
        //LERRORC("NetworkRendererClient", "lost connection");
      }
      catch (const NetworkError& err){
        //LERRORC("NetworkRendererClient", "network error");
      }
     _connectionMutex.unlock();
    return depth;
}

Core::Math::Vec3f MERConnection::getPosition(std::string electrodeIdentifier, int depth){
    Core::Math::Vec3f positionVec(-1234,-4321,1234);
    if(!_isConnected) return positionVec;

    std::string request = "POS:"+electrodeIdentifier+":"+std::to_string(depth);

    try{
         _connectionMutex.lock();
        sendString(request);

        BytePacket data = _connection->receive();
        if(data.byteArray().size() > 0){
            std::vector<float> pos;
            pos.resize(3);
            std::memcpy(&(pos[0]),data.byteArray().data(),3*sizeof(float));
            positionVec = Core::Math::Vec3f(pos[0],pos[1],pos[2]);
        }
      }
      catch (const ConnectionClosedError& err){
        //LERRORC("NetworkRendererClient", "lost connection");
      }
      catch (const NetworkError& err){
        //LERRORC("NetworkRendererClient", "network error");
      }
     _connectionMutex.unlock();
    return positionVec;
}

std::vector<double> MERConnection::getSignal(std::string electrodeIdentifier, int depth){
    std::vector<double> signal;

    if(!_isConnected) return signal;

    std::string request = "SIGNAL:"+electrodeIdentifier+":"+std::to_string(depth);

    try{
        _connectionMutex.lock();
        sendString(request);

        BytePacket data = _connection->receive();
        if(data.byteArray().size() > 0){
            signal.resize(data.byteArray().size() / sizeof(double));
            std::memcpy(&(signal[0]),data.byteArray().data(),data.byteArray().size());
        }
      }
      catch (const ConnectionClosedError& err){
        //LERRORC("NetworkRendererClient", "lost connection");
      }
      catch (const NetworkError& err){
        //LERRORC("NetworkRendererClient", "network error");
      }
    _connectionMutex.unlock();
    return signal;
}

void MERConnection::sendString(std::string msg){
    std::replace( msg.begin(), msg.end(), ',', '.');
    ByteArray stringArray;
    stringArray.append(msg.c_str(),msg.size());

    _connection->send(stringArray);
}
bool MERConnection::getIsConnected() const
{
    return _isConnected;
}

void MERConnection::clear(){
     _connectionMutex.lock();
    _connection = nullptr;
    _isConnected = false;
    _connectionMutex.unlock();
}

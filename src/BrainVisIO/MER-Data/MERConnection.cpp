#include "MERConnection.h"
#include <algorithm>
#include <Core/splittools.h>

#include <net/Error.h>

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
        sendString("GETELC");

        BytePacket data = _connection->receive();
        if(data.byteArray().size() > 0){
            std::string s(data.byteArray().data(),0,data.byteArray().size());
            electrodeList = splitString(s,':');
        }
      }
      catch (const ConnectionClosedError& err){
        //LERRORC("NetworkRendererClient", "lost connection");
      }
      catch (const NetworkError& err){
        //LERRORC("NetworkRendererClient", "network error");
      }

    return electrodeList;
}

int MERConnection::getCurrentDepth(std::string electrodeIdentifier){
    if(!_isConnected) return -20;

    return -10;
}

Core::Math::Vec3f MERConnection::getPosition(std::string electrodeIdentifier, int depth){
    if(!_isConnected) return Core::Math::Vec3f(-1234,-4321,1234);

    return Core::Math::Vec3f(0,0,0);
}

std::vector<double> MERConnection::getSignal(std::string electrodeIdentifier, int depth){
    std::vector<double> signal;

    if(!_isConnected) return signal;

    return signal;
}

void MERConnection::sendString(std::string msg){
    std::replace( msg.begin(), msg.end(), ',', '.');
    ByteArray stringArray;
    stringArray.append(msg.c_str(),msg.size());

    _connection->send(stringArray);
}

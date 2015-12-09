#ifndef MERCONNECTION
#define MERCONNECTION

#include <core/Math/Vectors.h>
#include <vector>
#include <mutex>

//NETWORKING
#include <base/Error.h>
#include <base/BytePacket.h>
#include <net/TCPNetworkAddress.h>
#include <net/LoopbackNetworkService.h>
#include <net/LoopbackConnectionListener.h>
#include <net/TCPNetworkService.h>
using namespace mocca;
using namespace mocca::net;

class MERConnection{
public:
    static MERConnection& getInstance(){
        static MERConnection instance;
        return instance;
    }

    void openConnection(std::string hostname, int port);
    void closeConnection();

    std::vector<std::string> getRegisteredElectrodes();
    int getCurrentDepth(std::string electrodeIdentifier);
    Core::Math::Vec3f getPosition(std::string electrodeIdentifier, int depth);
    std::vector<double> getSignal(std::string electrodeIdentifier, int depth);

    bool getIsConnected() const;

    void clear();

protected:
    MERConnection():_isConnected(false){};
    ~MERConnection(){};
private:
    void sendString(std::string msg);

    std::unique_ptr<AbstractConnection>     _connection;
    bool                                    _isConnected;

    std::mutex                              _connectionMutex;
};


#endif //NETWORKELECTRODE

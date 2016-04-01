#include <mocca/net/IStreamConnection.h>

#include <memory>
#include <vector>
#include <mutex>

#include <io/Data/MERBundle.h>
#include <thread>

using mocca::net::IStreamConnection;

struct MERElectrodeIds{
    MERElectrodeIds():
        _cen(2),_lat(3),_ant(4)
    {}
    MERElectrodeIds(int cen,int lat, int ant):
        _cen(cen),_lat(lat),_ant(ant)
    {}

  int _cen = 2;
  int _lat = 3;
  int _ant = 4;
};

static std::mutex merClientMutex;

class MERClient{
public:
    MERClient(int maxpackagesize);
    ~MERClient();

    void connect(std::string hostname, int port, MERElectrodeIds electrodeSettings);
    void connect(std::string hostnamePort, MERElectrodeIds electrodeSettings);

    void run();

    bool read();
    bool flush();

    void resetMERClient();

    bool isConnected();
    void setIsConnected(bool isConnected);

    int getCurrentDepth() const;
    void setCurrentDepth(int currentDepth);

    std::shared_ptr<BrainVisIO::MERData::MERBundle> getCurrentBundle() const;
    void setCurrentBundle(const std::shared_ptr<BrainVisIO::MERData::MERBundle> &currentBundle);

    bool getIsRecording() const;
    void setIsRecording(bool isRecording);

    void waitForThread();

private:

private:
    std::unique_ptr<IStreamConnection>      _connection;
    bool                                    _isConnected;
    MERElectrodeIds                         _electrodeSettings;

    int                                     _MAXPACKAGESIZE;

    bool                                                _isRecording;

    int                                                 _currentDepth;
    std::shared_ptr<BrainVisIO::MERData::MERBundle>     _currentBundle;

    std::unique_ptr<std::thread>                        _clientThread;
};

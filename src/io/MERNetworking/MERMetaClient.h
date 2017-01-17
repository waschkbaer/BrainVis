#include <mocca/net/IStreamConnection.h>
#include <mocca/net/ConnectionFactorySelector.h>
#include <mocca/net/NetworkError.h>
#include <mocca/base/Error.h>

#include <mocca/base/Thread.h>

#include <memory>
#include <vector>
#include <mutex>

#include <io/Data/MERBundle.h>
#include <thread>

using mocca::net::IStreamConnection;

class MERMetaClient : public mocca::Runnable {
public:
    MERMetaClient(const std::string& hostname,
                  uint16_t metaPort);
    ~MERMetaClient();

    int     getCurrentDepth();
    bool    isRecording();
    bool    isMeasuring();

private:
    void run();

    void readMetaData(mocca::ByteArray p);

    std::string _hostname;
    uint16_t    _port;
    std::unique_ptr<IStreamConnection>  _metaConnection;

    bool    _isRecording;
    bool    _isMeasuring;
    int     _currentDepth;
};

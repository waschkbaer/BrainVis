#include "io/MERNetworking/MERMetaClient.h"
#include "io/MERNetworking/MERMeta.h"

#include "mocca/log/LogManager.h"

using namespace mocca;
using namespace mocca::net;
using namespace std;
using namespace neuroio;

MERMetaClient::MERMetaClient(const std::string& hostname,
                             uint16_t metaPort):
_hostname(hostname),
_port(metaPort),
_metaConnection(nullptr),
_currentDepth(-10),
_isRecording(false){
ConnectionFactorySelector::addDefaultFactories();
}

MERMetaClient::~MERMetaClient(){

}

int MERMetaClient::getCurrentDepth(){
    return _currentDepth;
}

bool MERMetaClient::isRecording(){
    return _isRecording;
}

bool MERMetaClient::isMeasuring(){
    return _isMeasuring;
}

void MERMetaClient::run()
{
    bool _metaRunning = true;
    try{
        std::string metaAdd = _hostname+":"+std::to_string(_port);
        LINFO("MERClient | connect to : "<<metaAdd);
        _metaConnection = ConnectionFactorySelector::streamConnectionFactory("tcp").connect(metaAdd);
        LINFO("MERClient | connected");

        while(_metaRunning) {

        ByteArray package = _metaConnection->receive(50000);
        if(package.size() != 0 && package.size() % 10317 == 0)
        {
            for(int i = 0; i < package.size(); i += 10317){
                ByteArray subpackage;
                subpackage.append(package.data()+i,10317);

                readMetaData(std::move(subpackage));
            }
        }
        }
    }catch(const ConnectionClosedError& err){
        LERROR(err.what());
        _metaRunning = false;
    }catch(const NetworkError& err){
        LERROR(err.what());
        _metaRunning = false;
    }
    _metaRunning = false;
}

void MERMetaClient::readMetaData(mocca::ByteArray p)
{
    MERExtractor extractor;
    neuroio::MERMeta m = extractor.metaFromByteArray(p);

    LINFO("MERMetaClient | NEW META INFO");
    LINFO(" --  Recording: "<< m._recordingActive);
    LINFO(" --  Depth: "<< (m._depth/1000));
    LINFO(" --  Measuring: "<< m._measuringActive);

    _isRecording = m._recordingActive;
    _isMeasuring = m._measuringActive;
    _currentDepth = (m._depth/1000);
}

#include "MERServer.h"


#include "logging/logmanager.h"
#include "logging/consolelog.h"
#include "logging/htmllog.h"
#include "logging/textlog.h"
using namespace ghoul::logging;

int main(){
    LogManager::initialize(LogManager::LogLevel::Info, true);

    Log* html = new HTMLLog("MERDummy.html");
    Log* text = new TextLog("MERDummy.txt");
    Log* console = new ConsoleLog();
    LogMgr.addLog(html);
    LogMgr.addLog(text);
    LogMgr.addLog(console);


    MERServer* server = new MERServer(41234);

    server->startNetworkThread();

    while(true){
        server->run();
    }

    return 1;
}

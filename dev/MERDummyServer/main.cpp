#include "MERServer.h"
int main(){
    MERServer* server = new MERServer();
    while(true){
        server->run();
    }

    return 1;
}

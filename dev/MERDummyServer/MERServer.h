#ifndef MERSERVER
#define MERSERVER

#include <string>
#include <vector>
#include <Core/Math/Vectors.h>
#include <map>
#include <memory>
#include <thread>

//networking with mocca
#include <base/Error.h>
#include <base/BytePacket.h>
#include <net/TCPNetworkAddress.h>
#include <net/LoopbackNetworkService.h>
#include <net/LoopbackConnectionListener.h>
#include <net/TCPNetworkService.h>
#include <net/Error.h>

using namespace mocca;
using namespace mocca::net;

struct Data{

    Data():
    _signal(),
    _position(0,0,0),
    _depth(0){};

    std::vector<double> _signal;
    Core::Math::Vec3f   _position;
    int                 _depth;
};


class MERServer{
public:
    MERServer(int port);
    ~MERServer();

    void run();
    void startNetworkThread();



    void generateDataStepLeft();
    void generateDataStepRight();

    void printInformation();

protected:
    void networkerRun();
    void handleMsg(std::string s);


    void sendData();
    void listData();

    void initFakeData();

private:
    //networkcommands
    void getRegisteredElectrodes(); //retuns list of registered electrones to the client
    void getLatestDepth(std::string name); //sends the highest depth value of the electrode
    void getNextDepth(std::string name);

private:
    std::map<std::string,std::vector<Data>> _electrodes;


    std::unique_ptr<IConnectionListener>    _listener;
    std::unique_ptr<AbstractConnection>     _connection;

    std::unique_ptr<std::thread>            _networkThread;
};


static Core::Math::Vec3f LLatPos[15]{
    Core::Math::Vec3f(115.46f,100.15f,99.94f),
    Core::Math::Vec3f(115.19f,99.75f,100.82f),
    Core::Math::Vec3f(114.92f,99.35f,101.69f),
    Core::Math::Vec3f(114.64f,98.95f,102.57f),
    Core::Math::Vec3f(114.37f,98.56f,103.44f),
    Core::Math::Vec3f(114.09f,98.16f,104.32f),
    Core::Math::Vec3f(113.82f,97.76f,105.2f),
    Core::Math::Vec3f(113.55f,97.37f,106.07f),
    Core::Math::Vec3f(113.27f,96.97f,106.95f),
    Core::Math::Vec3f(113.0f,96.57f,107.82f),
    Core::Math::Vec3f(112.72f,96.17f,108.7f),
    Core::Math::Vec3f(112.45f,95.78f,109.57f),
    Core::Math::Vec3f(112.18f,95.38f,110.45f),
    Core::Math::Vec3f(111.9f,94.98f,111.33f),
    Core::Math::Vec3f(111.63f,94.58f,112.2f)
};

static Core::Math::Vec3f LCenPos[15]{
    Core::Math::Vec3f(113.54f,100.37f,99.44f),
    Core::Math::Vec3f(113.27f,99.98f,100.32f),
    Core::Math::Vec3f(112.99f,99.58f,101.19f),
    Core::Math::Vec3f(112.72f,99.18f,102.07f),
    Core::Math::Vec3f(112.44f,98.78f,102.94f),
    Core::Math::Vec3f(112.17f,98.39f,103.82f),
    Core::Math::Vec3f(111.9f,97.99f,104.7f),
    Core::Math::Vec3f(111.62f,97.59f,105.57f),
    Core::Math::Vec3f(111.35f,97.19f,106.45f),
    Core::Math::Vec3f(111.07f,96.8f,107.32f),
    Core::Math::Vec3f(110.8f,96.4f,108.2f),
    Core::Math::Vec3f(110.53f,96.0f,109.08f),
    Core::Math::Vec3f(110.25f,95.61f,109.95f),
    Core::Math::Vec3f(109.98f,95.21f,110.83f),
    Core::Math::Vec3f(109.7f,94.81f,111.7f)
};

static Core::Math::Vec3f LAntPos[15]{
    Core::Math::Vec3f(113.54f,102.19f,100.27f),
    Core::Math::Vec3f(113.27f,101.8f,101.14f),
    Core::Math::Vec3f(112.99f,101.4f,102.02f),
    Core::Math::Vec3f(112.72f,101.0f,102.9f),
    Core::Math::Vec3f(112.44f,100.61f,103.77f),
    Core::Math::Vec3f(112.17f,100.21f,104.65f),
    Core::Math::Vec3f(111.9f,99.81f,105.52f),
    Core::Math::Vec3f(111.62f,99.41f,106.4f),
    Core::Math::Vec3f(111.35f,99.02f,107.27f),
    Core::Math::Vec3f(111.07f,98.62f,108.15f),
    Core::Math::Vec3f(110.8f,98.22f,109.03f),
    Core::Math::Vec3f(110.53f,97.82f,109.9f),
    Core::Math::Vec3f(110.25f,97.43f,110.78f),
    Core::Math::Vec3f(109.98f,97.03f,111.65f),
    Core::Math::Vec3f(109.7f,96.63f,112.53f)
};

static Core::Math::Vec3f RLatPos[15]{
    Core::Math::Vec3f(81.76,99.73,101.34),
    Core::Math::Vec3f(82.12,99.37,102.2),
    Core::Math::Vec3f(82.48,99.01,103.06),
    Core::Math::Vec3f(82.83,98.65,103.93),
    Core::Math::Vec3f(83.19,98.29,104.79),
    Core::Math::Vec3f(83.55,97.93,105.65),
    Core::Math::Vec3f(83.9,97.57,106.51),
    Core::Math::Vec3f(84.26,97.21,107.37),
    Core::Math::Vec3f(84.62,96.85,108.23),
    Core::Math::Vec3f(84.97,96.49,109.1),
    Core::Math::Vec3f(85.33,96.12,109.96),
    Core::Math::Vec3f(85.69,95.76,110.82),
    Core::Math::Vec3f(86.05,95.4,111.68),
    Core::Math::Vec3f(86.4,95.04,112.54),
    Core::Math::Vec3f(86.76,94.68,113.41)
};

static Core::Math::Vec3f RCenPos[15]{
    Core::Math::Vec3f(83.63,100.01,100.68),
    Core::Math::Vec3f(83.99,99.64,101.54),
    Core::Math::Vec3f(84.35,99.28,102.41),
    Core::Math::Vec3f(84.7,98.92,103.27),
    Core::Math::Vec3f(85.06,98.56,104.13),
    Core::Math::Vec3f(85.42,98.2,104.99),
    Core::Math::Vec3f(85.77,97.84,105.85),
    Core::Math::Vec3f(86.13,97.48,106.71),
    Core::Math::Vec3f(86.49,97.12,107.58),
    Core::Math::Vec3f(86.84,96.76,108.44),
    Core::Math::Vec3f(87.2,96.4,109.3),
    Core::Math::Vec3f(87.56,96.04,110.16),
    Core::Math::Vec3f(87.91,95.68,111.02),
    Core::Math::Vec3f(88.27,95.32,111.89),
    Core::Math::Vec3f(88.63,94.96,112.75)

};

static Core::Math::Vec3f RAntPos[15]{
    Core::Math::Vec3f(83.63,101.85,101.45),
    Core::Math::Vec3f(83.99,101.49,102.32),
    Core::Math::Vec3f(84.35,101.13,103.18),
    Core::Math::Vec3f(84.7,100.77,104.04),
    Core::Math::Vec3f(85.06,100.41,104.9),
    Core::Math::Vec3f(85.42,100.05,105.76),
    Core::Math::Vec3f(85.77,99.69,106.62),
    Core::Math::Vec3f(86.13,99.33,107.49),
    Core::Math::Vec3f(86.49,98.97,108.35),
    Core::Math::Vec3f(86.84,98.61,109.21),
    Core::Math::Vec3f(87.2,98.25,110.07),
    Core::Math::Vec3f(87.56,97.88,110.93),
    Core::Math::Vec3f(87.91,97.52,111.8),
    Core::Math::Vec3f(88.27,97.16,112.66),
    Core::Math::Vec3f(88.63,96.8,113.52)

};

#endif

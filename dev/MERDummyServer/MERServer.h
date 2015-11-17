#ifndef MERSERVER
#define MERSERVER

#include <string>
#include <vector>
#include <Core/Math/Vectors.h>
#include <map>

struct Data{

    Data():
    _signal(),
    _position(0,0,0),
    _depth(0){};

    std::vector<double> _signal;
    Core::Math::Vec3f   _position;
    int8_t              _depth;
};


class MERServer{
public:
    MERServer();
    ~MERServer();

    void run();

    void updateData(std::string electrodeName);

protected:
    void generateData();
    void sendData();
    void listData();

    void initFakeData();

private:
    std::map<std::string,std::vector<Data>*> _electrodes;
};

#endif

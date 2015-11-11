#ifndef IELECTRODE
#define IELECTRODE

#include <vector>
#include <map>
#include <core/Math/Vectors.h>
#include "iMERData.h"

class iElectrode{
public:
    iElectrode(std::string name):_name(name){}
    virtual ~iElectrode(){}

    //return -1 if depth already stored
    virtual int8_t addData(int8_t depth, iMERData* data) = 0;
    virtual iMERData* getData(int8_t depth = 0) = 0;

protected:
    std::map<int8_t,iMERData*>   _electrodeData;
    std::string                 _name;

private:

};


#endif //IELECTRODE

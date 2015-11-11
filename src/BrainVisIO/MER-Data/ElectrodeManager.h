#ifndef ELECTRODEMANAGER
#define ELECTRODEMANAGER

#include <map>
#include "iElectrode.h"
#include <memory>

class ElectrodeManager{
public:
    ElectrodeManager();
    ~ElectrodeManager();

    std::shared_ptr<iElectrode> getElectrode(std::string name);
    std::shared_ptr<iElectrode> getElectrode(int i = 0);

    int getElectrodeCount() const;

    void addElectrode(std::shared_ptr<iElectrode> electrode);

protected:

private:
    std::map<std::string,std::shared_ptr<iElectrode>> _electrodes;
};


#endif //ELECTRODEMANAGER

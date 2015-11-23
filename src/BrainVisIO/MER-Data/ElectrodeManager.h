#ifndef ELECTRODEMANAGER
#define ELECTRODEMANAGER

#include <map>
#include "iElectrode.h"
#include <memory>

class ElectrodeManager{
public:
    static ElectrodeManager& getInstance(){
        static ElectrodeManager instance;
        return instance;
    }


    std::shared_ptr<iElectrode> getElectrode(std::string name);
    std::shared_ptr<iElectrode> getElectrode(int i = 0);

    int getElectrodeCount() const;

    void addElectrode(std::shared_ptr<iElectrode> electrode);

    void updateElectrodes();

    std::vector<std::string> getRegisteredElectrodes() const;

    void clear();

    std::string getTrackedElectrode() const;
    void setTrackedElectrode(const std::string &trackedElectrode);

protected:

private:
    ElectrodeManager();
    ~ElectrodeManager();

    std::map<std::string,std::shared_ptr<iElectrode>>   _electrodes;
    std::vector<std::string>                            _names;

    bool                                                _activeSide; //true = left, false = right

    std::string                                         _trackedElectrode;
};


#endif //ELECTRODEMANAGER

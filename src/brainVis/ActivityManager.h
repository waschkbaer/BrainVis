#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <string>

namespace BrainVis{

class ActivityManager{
public:
    static ActivityManager& getInstance(){
        static ActivityManager instance;
        return instance;
    }

    short getActiveDataset() const;
    void setActiveDataset(short activeDataset);

    short getActiveRenderer() const;
    void setActiveRenderer(short activeRenderer);

    std::string getActiveElectrode() const;
    void setActiveElectrode(const std::string &activeElectrode);

private:
    short               _activeDataset;
    short               _activeRenderer;
    std::string         _activeElectrode;

    ActivityManager():
    _activeDataset(-1),
    _activeRenderer(-1),
    _activeElectrode("none"){};
    ~ActivityManager(){};
};

}

#endif

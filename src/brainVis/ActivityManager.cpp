#include "ActivityManager.h"

using namespace BrainVis;

short ActivityManager::getActiveDataset() const
{
    return _activeDataset;
}

void ActivityManager::setActiveDataset(short activeDataset)
{
    _activeDataset = activeDataset;
}
short ActivityManager::getActiveRenderer() const
{
    return _activeRenderer;
}

void ActivityManager::setActiveRenderer(short activeRenderer)
{
    _activeRenderer = activeRenderer;
}
std::string ActivityManager::getActiveElectrode() const
{
    return _activeElectrode;
}

void ActivityManager::setActiveElectrode(const std::string &activeElectrode)
{
    _activeElectrode = activeElectrode;
}



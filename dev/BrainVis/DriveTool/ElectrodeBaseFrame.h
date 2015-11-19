#ifndef ELECTRODEBASEFRAME
#define ELECTRODEBASEFRAME

#include <QFrame>
#include <QLayout>
#include <string>
#include <memory>
#include <BrainVisIO/DataHandle.h>
#include <map>
#include "DriveEnums.h"


class ElectrodeBaseFrame: public QFrame{
public:
    ElectrodeBaseFrame(std::string electrodeName, QWidget* parent = 0);
    ~ElectrodeBaseFrame();

    void addElectrodeEntry(QWidget* entry);
    int childCount() const;

    void resetFrame();

    void createFrameEntrys(std::shared_ptr<DataHandle> data, ImageSetting setting);

    QImage* createFFTImage(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData, Core::Math::Vec2d powerRange);
    QImage* createSignalImage(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData);
    QImage* createProbabilityImaga(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData);

protected:
    QFrame* createSingleEntry(std::shared_ptr<DataHandle> data,std::string classy, int depth, Core::Math::Vec2d powerRange, ImageSetting setting);

private:
    int             _childCount;
    std::string     _electrodeName;

    std::map<int,QImage*> _spectralImages;
    std::map<int,QImage*> _signalImages;
    std::map<int,QImage*> _probabilityImages;

};

#endif

#ifndef ELECTRODEBASEFRAME
#define ELECTRODEBASEFRAME

#include <QFrame>
#include <QLayout>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>

#include "DriveEnums.h"

#include <string>
#include <memory>
#include <map>

#include <core/Math/Vectors.h>

class DataHandle;
class iMERData;

class ElectrodeBaseFrame: public QFrame{
    Q_OBJECT
public:
    ElectrodeBaseFrame(std::string electrodeName, QWidget* parent = 0);
    ~ElectrodeBaseFrame();

    void addElectrodeEntry(QWidget* entry);
    int childCount() const;

    void resetFrame();

    void createFrameEntrys(const std::shared_ptr<DataHandle> data,const ImageSetting& setting);
    void checkToDisableRadioButton();
    bool isCheckedForTracking();

    QImage* createFFTImage(const std::shared_ptr<DataHandle> data,const  std::shared_ptr<iMERData> eletrodeData,const Core::Math::Vec2d powerRange);
    QImage* createSignalImage(const std::shared_ptr<DataHandle> data,const  std::shared_ptr<iMERData> eletrodeData);
    QImage* createProbabilityImaga(const std::shared_ptr<DataHandle> data,const  std::shared_ptr<iMERData> eletrodeData);
    std::string electrodeName() const;

public slots:
    void on_radioButton_clicked();


protected:
    QFrame* createSingleEntry(const std::shared_ptr<DataHandle> data,const std::string& classy,const int depth,const Core::Math::Vec2d powerRange,const ImageSetting& setting);


private:
    int                 _childCount;
    std::string         _electrodeName;
    QRadioButton*       _isSelected;

    std::map<int,QImage*> _spectralImages;
    std::map<int,QImage*> _signalImages;
    std::map<int,QImage*> _probabilityImages;

};

#endif

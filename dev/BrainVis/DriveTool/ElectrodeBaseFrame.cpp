#include "ElectrodeBaseFrame.h"

#include <QLabel>
#include <QLineEdit>
#include <BrainVisIO/MER-Data/iElectrode.h>
#include <BrainVisIO/MER-Data/iMERData.h>

ElectrodeBaseFrame::ElectrodeBaseFrame(std::string electrodeName, QWidget* parent):
QFrame(parent),
_childCount(0),
_electrodeName(electrodeName){
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    setFrameStyle(QFrame::Sunken | QFrame::Box);
    setGeometry(0,0,225,30);
    setLayout(layout);
    setMaximumWidth(230);

    resetFrame();
}

ElectrodeBaseFrame::~ElectrodeBaseFrame(){
    while ( QWidget* w = findChild<QWidget*>() ){
        delete w;
    }
}


void ElectrodeBaseFrame::addElectrodeEntry(QWidget* entry){
    this->layout()->addWidget(entry);
    _childCount++;
}
int ElectrodeBaseFrame::childCount() const
{
    return _childCount;
}


void ElectrodeBaseFrame::resetFrame(){
    while ( QWidget* w = findChild<QWidget*>() ){
        delete w;
    }
    _childCount = 0;

    QLabel* textLabel = new QLabel(_electrodeName.c_str());
    QFont font("Arial",10,QFont::Bold);
    textLabel->setFont(font);
    this->layout()->addWidget(textLabel);
}


void ElectrodeBaseFrame::createFrameEntrys(std::shared_ptr<DataHandle> data){
    std::shared_ptr<iElectrode> electrode = data->getElectrode(_electrodeName);
    if(electrode == nullptr){
        return;
    }
    Vec2d range = electrode->getDepthRange();
    for(double d = range.x; d <= range.y;++d){
        QFrame* curFrame = createSingleEntry(data,(int)d,electrode->getSpectralPowerRange());
        this->addElectrodeEntry(curFrame);
    }
}

QFrame* ElectrodeBaseFrame::createSingleEntry(std::shared_ptr<DataHandle> data, int depth, Core::Math::Vec2d powerRange){
    std::shared_ptr<iElectrode> electrode = data->getElectrode(_electrodeName);
    if(electrode == nullptr){
        return NULL;
    }
    std::shared_ptr<iMERData> eletrodeData = electrode->getData(depth);
    if(eletrodeData == nullptr){
        return NULL;
    }
    Core::Math::Vec2d spectralRange = powerRange;

    QImage* image = new QImage(eletrodeData->getSpectralPower().size(),1,QImage::Format_RGB888);

    int colorIndex = 0;
    for(int c = 0; c < eletrodeData->getSpectralPower().size();++c){
         colorIndex = (int)((eletrodeData->getSpectralPower()[c]- spectralRange.x)/(spectralRange.y - spectralRange.x)*599);
         colorIndex = std::min(599,std::max(0,colorIndex));
         Vec3f color = data->getFFTColorImage()[colorIndex];

         image->setPixel(c,0,QColor((int)(color.x*255.0f),(int)(color.y*255.0f),(int)(color.z*255.0f)).rgb());
    }

    QHBoxLayout* baseLayout = new QHBoxLayout();
    baseLayout->setContentsMargins(0,0,0,0);

    QFrame* base = new QFrame();
    //base->setFrameStyle(QFrame::Sunken | QFrame::Box);
    base->setGeometry(0,0,225,30);
    base->setLayout(baseLayout);


    QLabel* imageL = new QLabel();
    imageL->setGeometry(0,0,175,30);
    imageL->setPixmap(QPixmap::fromImage(image->scaled(175,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    baseLayout->addWidget(imageL);

    QLineEdit* classification = new QLineEdit(QString(std::to_string(depth).c_str()));
    classification->setGeometry(0,0,50,30);
    baseLayout->addWidget(classification);

    return base;
}

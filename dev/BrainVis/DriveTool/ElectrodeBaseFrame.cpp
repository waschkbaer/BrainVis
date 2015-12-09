#include "ElectrodeBaseFrame.h"

#include <BrainVisIO/MER-Data/iElectrode.h>
#include <BrainVisIO/MER-Data/iMERData.h>
#include <cstring>

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
   ElectrodeManager::getInstance().getElectrode(_electrodeName)->setIsSelected(false);
}


void ElectrodeBaseFrame::addElectrodeEntry(QWidget* entry){
    this->layout()->addWidget(entry);
    _childCount++;
}
int ElectrodeBaseFrame::childCount() const
{
    return _childCount;
}
void ElectrodeBaseFrame::on_radioButton_clicked(){
    if(_isSelected->isChecked()){
        ElectrodeManager::getInstance().setTrackedElectrode(_electrodeName);
    }else{
        if( ElectrodeManager::getInstance().getTrackedElectrode() == _electrodeName){
            ElectrodeManager::getInstance().setTrackedElectrode("none");
        }
    }
}

void ElectrodeBaseFrame::checkToDisableRadioButton(){
    if( ElectrodeManager::getInstance().getTrackedElectrode() != _electrodeName){
            _isSelected->setChecked(false);
    }
}

bool ElectrodeBaseFrame::isCheckedForTracking(){
    return _isSelected->isChecked();
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

    _isSelected = new QRadioButton("track");
    this->layout()->addWidget(_isSelected);
    connect(_isSelected,SIGNAL(clicked()),this,SLOT(on_radioButton_clicked()));

    if( std::strcmp(ElectrodeManager::getInstance().getTrackedElectrode().c_str(),_electrodeName.c_str()) == 0){
            _isSelected->setChecked(true);
    }
}



void ElectrodeBaseFrame::createFrameEntrys(std::shared_ptr<DataHandle> data, ImageSetting setting){
    std::shared_ptr<iElectrode> electrode = data->getElectrode(_electrodeName);
    electrode->setIsSelected(true);
    if(electrode == nullptr){
        return;
    }
    Vec2d range = electrode->getDepthRange();
    for(double d = range.x; d <= range.y;++d){

        QFrame* curFrame = createSingleEntry(data,electrode->getData(d)->getClassification(),(int)d,electrode->getSpectralPowerRange(),setting);
        this->addElectrodeEntry(curFrame);
    }
}

QFrame* ElectrodeBaseFrame::createSingleEntry(std::shared_ptr<DataHandle> data,std::string classy, int depth, Core::Math::Vec2d powerRange, ImageSetting setting){
    std::shared_ptr<iElectrode> electrode = data->getElectrode(_electrodeName);
    if(electrode == nullptr){
        return NULL;
    }
    std::shared_ptr<iMERData> eletrodeData = electrode->getData(depth);
    if(eletrodeData == nullptr){
        return NULL;
    }
    Core::Math::Vec2d spectralRange = powerRange;

    QImage* image = NULL;


    switch(setting){
        case ImageSetting::FFT :        {
                                        if(_spectralImages.find(depth) == _spectralImages.end()){
                                            image = createFFTImage(data,eletrodeData,spectralRange);
                                            _spectralImages.insert(std::pair<int,QImage*>(depth,image));
                                        }else{
                                            image = _spectralImages.find(depth)->second;
                                        }
                                        break;
                                        }
        case ImageSetting::Signal :     {
                                        if(_signalImages.find(depth) == _signalImages.end()){
                                            image = createSignalImage(data,eletrodeData);
                                            _signalImages.insert(std::pair<int,QImage*>(depth,image));
                                        }else{
                                            image = _signalImages.find(depth)->second;
                                        }
                                        break;
                                        }
        case ImageSetting::probability :{
                                        break;
                                        }
    }


    QHBoxLayout* baseLayout = new QHBoxLayout();
    baseLayout->setContentsMargins(0,0,0,0);

    QFrame* base = new QFrame();
    //base->setFrameStyle(QFrame::Sunken | QFrame::Box);
    base->setGeometry(0,0,225,30);
    base->setLayout(baseLayout);

    std::string sDepth = std::to_string(depth);

    QLabel* depthLabel = new QLabel(QString(sDepth.c_str()));
    depthLabel->setGeometry(0,0,15,30);
    depthLabel->setMinimumWidth(15);
    baseLayout->addWidget(depthLabel);

    if(image != NULL){
        QLabel* imageL = new QLabel();
        imageL->setGeometry(0,0,175,30);
        imageL->setPixmap(QPixmap::fromImage(image->scaled(175,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
        baseLayout->addWidget(imageL);
    }

    QLineEdit* classification = new QLineEdit(QString(classy.c_str()));
    classification->setGeometry(0,0,50,30);
    baseLayout->addWidget(classification);

    return base;
}
std::string ElectrodeBaseFrame::electrodeName() const
{
    return _electrodeName;
}



QImage* ElectrodeBaseFrame::createFFTImage(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData, Core::Math::Vec2d powerRange){
    QImage* image = new QImage(eletrodeData->getSpectralPower().size(),1,QImage::Format_RGB888);

    int colorIndex = 0;
    for(int c = 0; c < eletrodeData->getSpectralPower().size();++c){
         colorIndex = (int)((eletrodeData->getSpectralPower()[c]- powerRange.x)/(powerRange.y - powerRange.x)*599);
         colorIndex = std::min(599,std::max(0,colorIndex));
         Vec3f color = data->getFFTColorImage()[colorIndex];

         image->setPixel(c,0,QColor((int)(color.x*255.0f),(int)(color.y*255.0f),(int)(color.z*255.0f)).rgb());
    }
    return image;
}
QImage* ElectrodeBaseFrame::createSignalImage(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData){

    if(eletrodeData->getInput().size() <= 0)  return NULL;

    Core::Math::Vec2d inputRange = eletrodeData->getInputRange();
    double center = (inputRange.x+inputRange.y)/2.0;
    double factor = inputRange.y-center;

    double step = eletrodeData->getInput().size()/175.0;

    QImage* image = new QImage(175,100,QImage::Format_RGB888);
    image->fill(QColor(255,255,255).rgb());
    double curValue = 0;
    int pixel = 0;
    int pixelMinusEins = 50;

    for(int c = 0; c < 175;++c){
        curValue = eletrodeData->getInput()[c*step];
        curValue = curValue-center;
        curValue = curValue/factor;
        pixel = 50+(curValue* 49);
        //std::cout <<c<< " "<< pixel<< std::endl;

        if(pixelMinusEins <= pixel){
            for(int i = pixelMinusEins; i < pixel;++i){
             image->setPixel(c,i,QColor(0,0,0).rgb());
            }
        }else{
            for(int i = pixel; i < pixelMinusEins;++i){
             image->setPixel(c,i,QColor(25,25,178).rgb());
            }
        }
        pixelMinusEins = pixel;
    }
    return image;
}
QImage* ElectrodeBaseFrame::createProbabilityImaga(std::shared_ptr<DataHandle> data, std::shared_ptr<iMERData> eletrodeData){
    return NULL;
}

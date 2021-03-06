#include "merimageentry.h"
#include "ui_merimageentry.h"

#include <QTimer>

#include "mertoolenums.h"
#include "merelectrodeentry.h"

#include <iostream>

#include "../ActivityManager.h"
#include <renderer/DICOMRenderManager.h>
#include <io/DataHandleManager.h>
#include <io/Data/MERBundleManager.h>

using namespace BrainVis;

MERimageentry::MERimageentry(int depth ,const std::string& name,std::shared_ptr<BrainVisIO::MERData::MERData> data,merelectrodeentry *parent) :
    QWidget(parent),
    _lastSpectralData(),
    _depth(depth),
    _name(name),
    _data(data),
    _entry(parent),
    ui(new Ui::MERimageentry)
{
    ui->setupUi(this);

    show();
    setUpdatesEnabled(true);
    ui->depthLabel->setText(QString(std::to_string(depth).c_str()));

    if(data->getIsSTNclassified()){
        ui->stnLine->setText("stn");
    }else{
        ui->stnLine->setText("");
    }

}

MERimageentry::~MERimageentry()
{
    delete ui;
}
bool MERimageentry::isSTN() const
{
    if(ui->stnLine->text().toStdString() == "stn" || ui->stnLine->text().toStdString() == "Stn" || ui->stnLine->text().toStdString() == "STN"){
        return true;
    }else{
        return false;
    }
}

void MERimageentry::setIsSTN(bool isSTN)
{
    if(isSTN){
        ui->stnLine->setText("stn");
    }else{
        ui->stnLine->setText("");
    }
}


void MERimageentry::createSpectralImage(const std::vector<double>& data){
    if(_image == nullptr ||
       _image->width() != ui->imgLabel->width() ||
       _image->height() != ui->imgLabel->height() ){
       _image = std::unique_ptr<QImage>(new QImage(160,ui->imgLabel->height(),QImage::Format_RGB888));
    }
    if(data.size() <= 0) return;

    _lastSpectralData = data;
    _image->fill(QColor(255,255,255).rgb());

    double value = 0;
    QColor col;
    for(int x = 0; x < _image->width();++x){
        value = (float)x/(float)(_image->width()-1) * (data.size()-1); //index

        value = data[(int)value];

        col = getSpectralColor(value);

        for(int y = 0; y < _image->height();++y){
            _image->setPixel(x,y,col.rgb());
        }
    }
    ui->imgLabel->setPixmap(QPixmap::fromImage(*_image));
}
void MERimageentry::createSignalImage(const std::vector<short>& data){
    QColor col(0,0,255);
    if(_image == nullptr ||
       _image->width() != ui->imgLabel->width() ||
       _image->height() != ui->imgLabel->height() ){
       _image = std::unique_ptr<QImage>(new QImage(ui->imgLabel->width(),ui->imgLabel->height(),QImage::Format_RGB888));
    }
    if(data.size() <= 0)return;

    _image->fill(QColor(255,255,255).rgb());

    int32_t minPossible = -32767;
    int32_t maxImgDistance = 65535 ;

    float signalIndex = 0;         //castintlater
    float signalIndexNext = 0;         //castintlater
    double signalValue = 0;
    int32_t prevValue = 0;
    float percentage = 0.0f;

    float scale = 100.0f;

    for(int x = 0; x < _image->width();++x){
        signalIndex = (float)x /(float)_image->width();
        signalIndex = signalIndex * data.size();

        signalIndexNext = (float)(x+1) /(float)_image->width();
        signalIndexNext = signalIndexNext * data.size();

        for(int xi = signalIndex; xi < signalIndexNext;++xi){

            signalValue = data[xi];

            //get in range!
            signalValue = (signalValue* scale) - minPossible;

            //get in ImageRange
            percentage = (float)signalValue/(float)maxImgDistance;
            signalValue = (_image->height()-1) * percentage;

            //take care to be really in imgspace
            signalValue = std::max(0,std::min((int)signalValue,_image->height()-1));

            int delta = (signalValue-_image->height()/2);
            delta = std::abs(delta);

            if(x > 1){
                if(signalValue > prevValue){
                    for(int y = prevValue; y < signalValue;y++){
                        _image->setPixel(x,y,col.rgb());
                    }
                }else{
                    for(int y = signalValue; y < prevValue;y++){
                        _image->setPixel(x,y,col.rgb());
                    }
                }
            }

            prevValue = signalValue;
            }
       }
    ui->imgLabel->setPixmap(QPixmap::fromImage(*_image));
}

void MERimageentry::update(){

}

QColor MERimageentry::getSpectralColor(double value){
    int r,g,b;

       r = value * 255.0f;
       b = (1.0-value)* 255.0f;

       if(value > 0.5){
           g = (1-value)/0.5 * 255.0f;
       }else{
           g = value/0.5*255.0f;
       }

       r = std::max(0,std::min(255,r));
       g = std::max(0,std::min(255,g));
       b = std::max(0,std::min(255,b));

       return QColor(r,g,b);
}

void MERimageentry::mousePressEvent(QMouseEvent* event){
    BrainVisIO::MERData::MERBundleManager::getInstance().deselectAllData();
    _entry->disableSelection();
    setSelected(true);
    uint16_t handle = ActivityManager::getInstance().getActiveDataset();
    std::shared_ptr<DataHandle> d = DataHandleManager::getInstance().getDataHandle(handle);
    if(d != nullptr)
        d->setFocusPoint(_data->getPosition());
}

void MERimageentry::on_stnLine_textChanged(const QString &arg1)
{
    if(arg1.toStdString() == "stn" || arg1.toStdString() == "Stn" || arg1.toStdString() == "STN"){
        ui->stnLine->setStyleSheet("background-color:white; color:black;");
        _data->setIsSTNclassified(true);
    }else{
        _data->setIsSTNclassified(false);
        ui->stnLine->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4d4d4d, stop: 0 #646464, stop: 1 #5d5d5d);");
    }
}

void MERimageentry::setSelected(bool selected){
    _data->setFocusSelected(selected);
    if(_data->getFocusSelected()){
        ui->imgLabel->setStyleSheet("border: 2px solid white");
    }else{
        ui->imgLabel->setStyleSheet("border: 0px");
    }
}

#include "merimageentry.h"
#include "ui_merimageentry.h"

#include "mertoolenums.h"

#include <iostream>

MERimageentry::MERimageentry(int depth ,const std::string& name,QWidget *parent) :
    QWidget(parent),
    _lastSpectralData(),
    _depth(depth),
    _name(name),
    ui(new Ui::MERimageentry)
{
    ui->setupUi(this);

    show();
    setUpdatesEnabled(true);
}

MERimageentry::~MERimageentry()
{
    delete ui;
}
bool MERimageentry::isSTN() const
{
    return ui->stnBox->isChecked();
}

void MERimageentry::setIsSTN(bool isSTN)
{
    ui->stnBox->setChecked(isSTN);
}


void MERimageentry::createSpectralImage(const std::vector<double>& data){
    if(_image == nullptr ||
       _image->width() != ui->imgLabel->width() ||
       _image->height() != ui->imgLabel->height() ){
       _image = std::unique_ptr<QImage>(new QImage(ui->imgLabel->width(),ui->imgLabel->height(),QImage::Format_RGB888));
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
   // emit clicked();
    std::cout << "here"<< _depth << "  "<< _name<<std::endl;
}

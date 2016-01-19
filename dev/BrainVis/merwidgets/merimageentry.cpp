#include "merimageentry.h"
#include "ui_merimageentry.h"

#include <iostream>

MERimageentry::MERimageentry(QWidget *parent) :
    QWidget(parent),
    _lastSpectralData(),
    ui(new Ui::MERimageentry)
{
    ui->setupUi(this);

    show();
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
        value = data[value];

        col = getSpectralColor(value);

        for(int y = 0; y < _image->height();++y){
            _image->setPixel(x,y,col.rgb());
        }
    }
    ui->imgLabel->setPixmap(QPixmap::fromImage(*_image));
}
void MERimageentry::createSignalImage(const std::vector<short>& data){

}

void MERimageentry::update(){
    createSpectralImage(_lastSpectralData);
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

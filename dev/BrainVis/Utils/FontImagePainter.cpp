#include "FontImagePainter.h"


FontImagePainter::FontImagePainter():
    FontImagePainter(640,480){

}

FontImagePainter::FontImagePainter(int width, int height){
    _image = std::make_shared<QImage>(width,height,QImage::Format_RGB888);
    _image->fill(QColor(0,0,0).rgb());

    _painter = std::make_shared<QPainter>(_image.get());

    QPen myPen(Qt::white, 2, Qt::SolidLine);
    _painter->setPen(myPen);
}

FontImagePainter::~FontImagePainter(){

}

void FontImagePainter::clearImage(){
    _image->fill(QColor(0,0,0).rgb());
}

void FontImagePainter::drawText(int x, int y, std::string text){
    return _painter->drawText(x,y,QString(text.c_str()));
}

uchar* FontImagePainter::getImageData(){
    return _image->bits();
}

Core::Math::Vec2ui FontImagePainter::getImageSize(){
    return Core::Math::Vec2ui(_image->width(),_image->height());
}

uint8_t FontImagePainter::getSizePerPixel(){
    return 3;
}

void FontImagePainter::saveImage(std::string filename){
    _image->save(QString(filename.c_str()));
}

void FontImagePainter::resizeImage(int width, int height){
    _image = std::make_shared<QImage>(width,height,QImage::Format_RGB888);
    _image->fill(QColor(0,0,0).rgb());
}

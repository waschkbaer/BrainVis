#include "FontImagePainter.h"


FontImagePainter::FontImagePainter():
    FontImagePainter(640,480){

}

FontImagePainter::FontImagePainter(int width, int height){
    //_image = std::make_shared<QImage>(width,height,QImage::Format_RGB888);
    _image = new QImage(width,height,QImage::Format_RGB888);
    _image->fill(QColor(0,0,0).rgb());

    _painter = new QPainter(_image);

    QPen myPen(Qt::white, 2, Qt::SolidLine);
    _painter->setPen(myPen);
    _rawData.resize(_image->width()*_image->height()*3);
    _rawData.clear();
}

FontImagePainter::~FontImagePainter(){
}

void FontImagePainter::clearImage(){
    _image->fill(QColor(0,0,0).rgb());
}

void FontImagePainter::drawText(int x, int y, std::string text){
    return _painter->drawText(x,y,QString(text.c_str()));
}

char* FontImagePainter::getImageData(){
    return &(_rawData[0]);
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

    delete _painter;
    delete _image;
    _image = new QImage(width,height,QImage::Format_RGB888);
    _image->fill(QColor(0,0,0).rgb());
    _painter = new QPainter(_image);
    QPen myPen(Qt::white, 2, Qt::SolidLine);
    _painter->setPen(myPen);
}

void FontImagePainter::setFontColor(int r, int g, int b){

}

void FontImagePainter::finishText(){
    _rawData.resize(_image->width()*_image->height()*3);
    char* curLineStart = (char*) _image->bits();


    for(int i = 0; i < _image->height();++i){
        std::memcpy(&(_rawData[i*(_image->width()*3)]),curLineStart,_image->width()*3);

        curLineStart += _image->bytesPerLine()*sizeof(char);
    }

}

#ifndef FONTIMAGEPAINTER
#define FONTIMAGEPAINTER

#include <QPainter>
#include <QImage>
#include <QColor>
#include <memory>

#include <string>
#include <Core/Math/Vectors.h>

class FontImagePainter{
public:
    FontImagePainter();
    FontImagePainter(int width, int height);
    ~FontImagePainter();

    void clearImage();
    void drawText(int x, int y, std::string text);

    uchar* getImageData();
    Core::Math::Vec2ui getImageSize();
    uint8_t getSizePerPixel();

    void setFontColor(int r, int g, int b);

    void saveImage(std::string filename);
    void resizeImage(int width, int height);

private:
    QImage*                     _image;
    QPainter*                   _painter;
    QColor                      _penColor;
protected:
};

#endif

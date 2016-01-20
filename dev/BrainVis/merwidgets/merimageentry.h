#ifndef MERIMAGEENTRY_H
#define MERIMAGEENTRY_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <memory>

#include "mertoolenums.h"

namespace Ui {
class MERimageentry;
}

class MERimageentry : public QWidget
{
    Q_OBJECT

public:
    explicit MERimageentry(int depth = -10,const std::string& name = "none",QWidget *parent = 0);
    ~MERimageentry();

    void createSpectralImage(const std::vector<double>& data);
    void createSignalImage(const std::vector<short>& data);

    bool isSTN() const;
    void setIsSTN(bool isSTN);

    void update();

    void mousePressEvent(QMouseEvent* event);

private:
    QColor getSpectralColor(double value);

    Ui::MERimageentry *ui;

    std::unique_ptr<QImage> _image;
    std::vector<double>     _lastSpectralData;

    int                     _depth;
    std::string             _name;
};

#endif // MERIMAGEENTRY_H

#ifndef MERIMAGEENTRY_H
#define MERIMAGEENTRY_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <memory>

namespace Ui {
class MERimageentry;
}

class MERimageentry : public QWidget
{
    Q_OBJECT

public:
    explicit MERimageentry(QWidget *parent = 0);
    ~MERimageentry();

    void createSpectralImage(const std::vector<double>& data);
    void createSignalImage(const std::vector<short>& data);

    bool isSTN() const;
    void setIsSTN(bool isSTN);

    void update();

private:
    QColor getSpectralColor(double value);

    Ui::MERimageentry *ui;

    std::unique_ptr<QImage> _image;
    std::vector<double>     _lastSpectralData;
};

#endif // MERIMAGEENTRY_H

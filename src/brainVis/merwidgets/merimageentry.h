#ifndef MERIMAGEENTRY_H
#define MERIMAGEENTRY_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <memory>

#include "mertoolenums.h"
#include <io/Data/MERData.h>

namespace Ui {
class MERimageentry;
}
class merelectrodeentry;

class MERimageentry : public QWidget
{
    Q_OBJECT

public:
    explicit MERimageentry(int depth = -10,const std::string& name = "none",std::shared_ptr<BrainVisIO::MERData::MERData> data = nullptr,merelectrodeentry *parent = 0);
    ~MERimageentry();

    void createSpectralImage(const std::vector<double>& data);
    void createSignalImage(const std::vector<short>& data);

    bool isSTN() const;
    void setIsSTN(bool isSTN);

    void update();

    void mousePressEvent(QMouseEvent* event);

    void setSelected(bool selected);

private slots:

    void on_stnLine_textChanged(const QString &arg1);

private:
    QColor getSpectralColor(double value);
    merelectrodeentry*          _entry;

    Ui::MERimageentry *ui;

    std::unique_ptr<QImage> _image;
    std::vector<double>     _lastSpectralData;

    int                     _depth;
    std::string             _name;

    std::shared_ptr<BrainVisIO::MERData::MERData> _data;
};

#endif // MERIMAGEENTRY_H

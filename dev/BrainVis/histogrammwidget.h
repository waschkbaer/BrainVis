#ifndef HISTOGRAMMWIDGET_H
#define HISTOGRAMMWIDGET_H

#include <QDockWidget>
#include <QPainter>
#include <QColor>

#include <vector>
#include <BrainVisIO/DataHandle.h>
#include <memory>

namespace Ui {
class HistogrammWidget;
}

class HistogrammWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit HistogrammWidget(QWidget *parent = 0, std::shared_ptr<DataHandle> data = nullptr);
    ~HistogrammWidget();

    void createHistogramms(std::vector<uint16_t> ctHistogramm, std::vector<uint16_t> mrHistogramm);

    void closeEvent(QCloseEvent *bar);

private slots:
    void update();

private:
    QImage* createHistogramm(std::vector<uint16_t> histogramm);
    void drawTF(QImage* image, int width, int height, float pos, float grad);

    Ui::HistogrammWidget *ui;
    std::shared_ptr<DataHandle> _data;
    QImage* _ctImage;
    QImage* _mrImage;

    QPainter*                   _painter;
    QColor                      _penColor;

    float lastPos;
    float lastGrad;
};

#endif // HISTOGRAMMWIDGET_H

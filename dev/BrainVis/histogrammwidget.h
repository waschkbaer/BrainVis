#ifndef HISTOGRAMMWIDGET_H
#define HISTOGRAMMWIDGET_H

#include <QDockWidget>

#include <vector>
#include <BrainVisIO/DataHandle.h>

namespace Ui {
class HistogrammWidget;
}

class HistogrammWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit HistogrammWidget(QWidget *parent = 0);
    ~HistogrammWidget();

    void createHistogramms(std::vector<uint16_t> ctHistogramm, std::vector<uint16_t> mrHistogramm);

private:
    Ui::HistogrammWidget *ui;




    QImage* createHistogramm(std::vector<uint16_t> histogramm);
};

#endif // HISTOGRAMMWIDGET_H

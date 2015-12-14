#include "histogrammwidget.h"
#include "ui_histogrammwidget.h"

HistogrammWidget::HistogrammWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::HistogrammWidget)
{
    ui->setupUi(this);

    setFloating(true);

    show();
}

HistogrammWidget::~HistogrammWidget()
{
    delete ui;
}


void HistogrammWidget::createHistogramms(std::vector<uint16_t> ctHistogramm, std::vector<uint16_t> mrHistogramm){
    QImage* ct = createHistogramm(ctHistogramm);
    QImage* mr = createHistogramm(mrHistogramm);


    ui->ctImgLabel->setPixmap(QPixmap::fromImage(ct->scaled(491,77,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    ui->mriImgLabel->setPixmap(QPixmap::fromImage(mr->scaled(491,77,Qt::IgnoreAspectRatio,Qt::FastTransformation)));

}

QImage* HistogrammWidget::createHistogramm(std::vector<uint16_t> histogramm){
    float maxValue = 0;
    for(int i = 0; i < histogramm.size();++i){
        if(maxValue < (float)histogramm[i]) maxValue =  (float)histogramm[i];
    }

    QImage* image = new QImage(histogramm.size(),300,QImage::Format_RGB888);

    int currentHeight = 0.0f;
    for(int x = 0; x < histogramm.size();++x){
        currentHeight = (int) ( ( (float)histogramm[x] / maxValue ) * 299 );

        for(int y = 0; y < currentHeight;++y){
            image->setPixel(x,(299-y),QColor(128,128,128).rgb());
        }
    }

    return image;
}

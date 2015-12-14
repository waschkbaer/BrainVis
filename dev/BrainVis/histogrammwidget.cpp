#include "histogrammwidget.h"
#include "ui_histogrammwidget.h"

#include <QTimer>

HistogrammWidget::HistogrammWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    ui(new Ui::HistogrammWidget),
    _ctImage(NULL),
    _mrImage(NULL),
    _data(data)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    lastPos = _data->getPosition();
    lastGrad = _data->getGradient();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(500);
}

HistogrammWidget::~HistogrammWidget()
{
    delete ui;
}


void HistogrammWidget::createHistogramms(std::vector<uint16_t> ctHistogramm, std::vector<uint16_t> mrHistogramm){
    if(_ctImage != NULL){
        delete _ctImage;
    }

    if(_mrImage != NULL){
        delete _mrImage;
    }

    _ctImage = createHistogramm(ctHistogramm);
    _mrImage = createHistogramm(mrHistogramm);


    ui->ctImgLabel->setPixmap(QPixmap::fromImage(_ctImage->scaled(491,77,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    ui->mriImgLabel->setPixmap(QPixmap::fromImage(_mrImage->scaled(491,77,Qt::IgnoreAspectRatio,Qt::FastTransformation)));

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

    drawTF(image,histogramm.size(),300,_data->getPosition(),_data->getGradient());

    return image;
}

float Smoothstep(float x) {
  return 3*x*x-2*x*x*x;
}

void HistogrammWidget::drawTF(QImage* image, int width, int height, float pos, float grad){
    size_t iCenterPoint = size_t((width-1) * float(std::min<float>(std::max<float>(0,pos),1)));
    size_t iInvGradient = size_t((width-1) * float(std::min<float>(std::max<float>(0,grad),1)));


    size_t iRampStartPoint = (iInvGradient/2 > iCenterPoint) ? 0 : iCenterPoint-(iInvGradient/2);
    size_t iRampEndPoint   = (iInvGradient/2 + iCenterPoint > width)  ? width : iCenterPoint+(iInvGradient/2);


    for (size_t i = 0;i<iRampStartPoint;i++)
      image->setPixel(i,height-1,QColor(0,0,0).rgb());

    for (size_t i = iRampStartPoint;i<iRampEndPoint;i++) {
      float fValue = Smoothstep(float(i-iCenterPoint+(iInvGradient/2))/float(iInvGradient));
      fValue*= (height-1);
      image->setPixel(i,(height-1)-fValue,QColor(0,0,0).rgb());
    }

    for (size_t i = iRampEndPoint;i<width;i++)
      image->setPixel(i,0,QColor(0,0,0).rgb());
}

void HistogrammWidget::update(){
    if(lastGrad != _data->getGradient() || lastPos != _data->getPosition()){
        createHistogramms(_data->getCTHistogramm(),_data->getMRHistogramm());
    }
}

#include "histogrammwidget.h"
#include "ui_histogrammwidget.h"

#include "mainwindow.h"

#include <QTimer>

HistogrammWidget::HistogrammWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    ui(new Ui::HistogrammWidget),
    _ctImage(NULL),
    _mrImage(NULL),
    _painter(NULL),
    _data(data)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    lastPos = _data->getPosition();
    lastGrad = _data->getGradient();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(100);
}

HistogrammWidget::~HistogrammWidget()
{
    if(_painter != NULL) delete _painter;
    if(_ctImage != NULL) delete _ctImage;
    if(_mrImage != NULL) delete _mrImage;
    delete ui;
}


void HistogrammWidget::createHistogramms(std::vector<uint16_t> ctHistogramm, std::vector<uint16_t> mrHistogramm){
    if(_painter != NULL) delete _painter;
    if(_ctImage != NULL) delete _ctImage;

    _ctImage = createHistogramm(ctHistogramm);

    if(_painter != NULL) delete _painter;
    if(_mrImage != NULL) delete _mrImage;

    _mrImage = createHistogramm(mrHistogramm);


    ui->ctImgLabel->setPixmap(QPixmap::fromImage(_ctImage->scaled(716,153,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    ui->mriImgLabel->setPixmap(QPixmap::fromImage(_mrImage->scaled(716,153,Qt::IgnoreAspectRatio,Qt::FastTransformation)));

}

QImage* HistogrammWidget::createHistogramm(std::vector<uint16_t> histogramm){
    float maxValue = 0;
    for(int i = 0; i < histogramm.size();++i){
        if(maxValue < (float)histogramm[i]) maxValue =  (float)histogramm[i];
    }

    QImage* image = new QImage(716,153,QImage::Format_RGB888);
    image->fill(QColor(255,255,255).rgb());


    int currentHeight = 0.0f;
    int currentIndex = 0;
    for(int x = 0; x < 716;++x){
        currentIndex = x* histogramm.size()/716;

        currentHeight = (int) ( ( (float)histogramm[currentIndex] / maxValue ) * 152 );

        for(int y = 0; y < currentHeight;++y){
            image->setPixel(x,(152-y),QColor(75,75,75).rgb());
        }
    }

    drawTF(image,716,153,_data->getPosition(),_data->getGradient());



    _painter = new QPainter(image);

    QPen myPen(Qt::red, 2, Qt::SolidLine);
    _painter->setPen(myPen);

    _painter->drawText(0,10,QString("0"));

    std::string maxString = std::to_string(histogramm.size());
    _painter->drawText(716-25,10,QString(maxString.c_str()));


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

void HistogrammWidget::closeEvent(QCloseEvent *bar){
    MainWindow* parent = (MainWindow*)this->parent();
    parent->closeHistogrammWidget();
}

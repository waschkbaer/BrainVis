#include "drivewidget.h"
#include "ui_drivewidget.h"
#include <QLabel>

DriveWidget::DriveWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    _data(data),
    ui(new Ui::DriveWidget)
{
    ui->setupUi(this);
    this->setFloating(true);

    std::vector<std::string> elektrodes = ElectrodeManager::getInstance().getRegisteredElectrodes();

    for(int j = 0; j < elektrodes.size();++j){
        QVBoxLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0,0,0,0);

        QFrame *frame = new QFrame();
        frame->setFrameStyle(QFrame::Sunken | QFrame::Box);
        frame->setGeometry(0,0,100,15);
        frame->setLayout(layout);


        for(int i = -10; i < 4;++i){
            QFrame *dataEntry = createElectordeImageEntry(elektrodes[j],i);
            layout->addWidget(dataEntry);
        }

        if(j <= 2){
            ui->DataFrame->layout()->addWidget(frame);
        }else{
            ui->DataFrameRight->layout()->addWidget(frame);
        }

    }


    //spectrum image
    QImage* image = new QImage(_data->getFFTColorImage().size(),1,QImage::Format_RGB888);
    for(int c = 0; c < _data->getFFTColorImage().size();++c){
         Vec3f color = _data->getFFTColorImage()[c];
         image->setPixel(c,0,QColor((int)(color.x*255.0f),(int)(color.y*255.0f),(int)(color.z*255.0f)).rgb());
    }
    QLabel* imageL = new QLabel();
    imageL->setGeometry(0,0,400,30);
    imageL->setPixmap(QPixmap::fromImage(image->scaled(400,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    ui->colorFrame->layout()->addWidget(imageL);
    QLabel* imageR = new QLabel();
    imageR->setGeometry(0,0,400,30);
    imageR->setPixmap(QPixmap::fromImage(image->scaled(400,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    ui->colorFrameRight->layout()->addWidget(imageR);

    ui->verticalSlider->setValue(0);
}

DriveWidget::~DriveWidget()
{
    delete ui;
}

void DriveWidget::on_verticalSlider_sliderMoved(int position)
{
    _data->setDisplayedDriveRange(Vec2i(-10,position));
}


void DriveWidget::addTrajectory(std::string name, std::shared_ptr<iElectrode> electrode){

}
void DriveWidget::updateTrajectory(std::string name){

}


QFrame* DriveWidget::createElectordeImageEntry(std::string name, int depth){
    if(_data->getElectrode(name) != nullptr){
        std::shared_ptr<iElectrode> traj = _data->getElectrode(name);

        if(depth >= -10 && depth <= 4){
            std::shared_ptr<iMERData> eletrodeData = traj->getData(depth);
            if(eletrodeData == nullptr){
                return NULL;
            }
            QImage* image = new QImage(eletrodeData->getSpectralPower().size(),1,QImage::Format_RGB888);

            int colorIndex = 0;
            for(int c = 0; c < eletrodeData->getSpectralPower().size();++c){
                //colorIndex = (int)((eletrodeData->spectralFlow()[c]-_data->getSpectralRange().x)/(_data->getSpectralRange().y-_data->getSpectralRange().x)*599);
                 colorIndex = (int)((eletrodeData->getSpectralPower()[c]-traj->getSpectralPowerRange().x)/(traj->getSpectralPowerRange().y - traj->getSpectralPowerRange().x)*599);
                 colorIndex = std::min(599,std::max(0,colorIndex));
                 Vec3f color = _data->getFFTColorImage()[colorIndex];

                 image->setPixel(c,0,QColor((int)(color.x*255.0f),(int)(color.y*255.0f),(int)(color.z*255.0f)).rgb());
            }

            QHBoxLayout* baseLayout = new QHBoxLayout();
            baseLayout->setContentsMargins(0,0,0,0);

            QFrame* base = new QFrame();
            //base->setFrameStyle(QFrame::Sunken | QFrame::Box);
            base->setGeometry(0,0,100,15);
            base->setLayout(baseLayout);

            QLabel* imageL = new QLabel();
            imageL->setGeometry(0,0,150,20);
            imageL->setPixmap(QPixmap::fromImage(image->scaled(150,20,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
            baseLayout->addWidget(imageL);

            std::string naming = "("+std::to_string(depth) + ") "+name;

            QLabel* textL = new QLabel(naming.c_str());
            textL->setGeometry(0,0,100,75);
            baseLayout->addWidget(textL);

            return base;

        }
    }
    return NULL;
}

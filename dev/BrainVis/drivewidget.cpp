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

    /*QImage image(100,100,QImage::Format_RGB888);
    QColor c(255,255,255);
    for(int x = 0; x < 100; x+=2){
        for(int y = 0; y < 100; y+=2){
            image.setPixel(x,y,c.rgba());
        }
    }
    ui->testLabel->setPixmap(QPixmap::fromImage(image.scaled(300,100,Qt::IgnoreAspectRatio,Qt::FastTransformation)));*/

    addTrajectory(_data->getLeftBundle()->getTrajectory(0)->getName(),_data->getLeftBundle()->getTrajectory(0));
    addTrajectory(_data->getLeftBundle()->getTrajectory(1)->getName(),_data->getLeftBundle()->getTrajectory(1));
    addTrajectory(_data->getLeftBundle()->getTrajectory(2)->getName(),_data->getLeftBundle()->getTrajectory(2));
    addTrajectory(_data->getRightBundle()->getTrajectory(0)->getName(),_data->getRightBundle()->getTrajectory(0));
    addTrajectory(_data->getRightBundle()->getTrajectory(1)->getName(),_data->getRightBundle()->getTrajectory(1));
    addTrajectory(_data->getRightBundle()->getTrajectory(2)->getName(),_data->getRightBundle()->getTrajectory(2));

    std::vector<std::string> elektrodes;
    elektrodes.push_back("LLat");
    elektrodes.push_back("LCen");
    elektrodes.push_back("LAnt");
    elektrodes.push_back("RLat");
    elektrodes.push_back("RCen");
    elektrodes.push_back("RAnt");

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


    ui->verticalSlider->setValue(_data->getLeftBundle()->getTrajectory(0)->getDepthRange().y);
}

DriveWidget::~DriveWidget()
{
    delete ui;
}

void DriveWidget::on_verticalSlider_sliderMoved(int position)
{
    _data->setDisplayedDriveRange(Vec2i(-10,position));
}


void DriveWidget::addTrajectory(std::string name, std::shared_ptr<AbstrElectrode> trajectory){
    if(_trajectoryData.find(name) == _trajectoryData.end()){
        _trajectoryData.insert(std::pair<std::string, std::shared_ptr<AbstrElectrode>>(name,trajectory));
    }
}
void DriveWidget::updateTrajectory(std::string name){
    if(_trajectoryData.find(name) != _trajectoryData.end()){
        std::shared_ptr<AbstrElectrode> traj = _trajectoryData.find(name)->second;
    }

}


QFrame* DriveWidget::createElectordeImageEntry(std::string name, int depth){
    if(_trajectoryData.find(name) != _trajectoryData.end()){
        std::shared_ptr<AbstrElectrode> traj = _trajectoryData.find(name)->second;

        if(depth >= traj->getDepthRange().x && depth <= traj->getDepthRange().y){
            std::shared_ptr<MERData> eletrodeData = traj->getData(depth);
            if(eletrodeData == nullptr){
                return NULL;
            }
            QImage* image = new QImage(eletrodeData->spectralFlow().size(),1,QImage::Format_RGB888);

            int colorIndex = 0;
            std::cout <<"------------specrange--"<< _data->getSpectralRange() << std::endl;
            for(int c = 0; c < eletrodeData->spectralFlow().size();++c){
                //colorIndex = (int)((eletrodeData->spectralFlow()[c]-_data->getSpectralRange().x)/(_data->getSpectralRange().y-_data->getSpectralRange().x)*599);
                 colorIndex = (int)((eletrodeData->spectralFlow()[c]-traj->getElektrodeRange().x)/(traj->getElektrodeRange().y - traj->getElektrodeRange().x)*599);
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

            std::string naming = ">"+ std::to_string(depth) + " "+name;

            QLabel* textL = new QLabel(naming.c_str());
            textL->setGeometry(0,0,100,75);
            baseLayout->addWidget(textL);

            return base;

        }
    }
    return NULL;
}

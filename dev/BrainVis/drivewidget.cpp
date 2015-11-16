#include "drivewidget.h"
#include "ui_drivewidget.h"
#include <QLabel>
#include <QLineEdit>

DriveWidget::DriveWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    _data(data),
    ui(new Ui::DriveWidget)
{
    ui->setupUi(this);
    this->setFloating(true);

    std::vector<std::string> elektrodes = ElectrodeManager::getInstance().getRegisteredElectrodes();

    for(int j = 0; j < elektrodes.size();++j){
        ui->electrodeSelection->addItem(elektrodes[j].c_str());
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
    //ui->colorFrame->layout()->addWidget(imageL);
    QLabel* imageR = new QLabel();
    imageR->setGeometry(0,0,400,30);
    imageR->setPixmap(QPixmap::fromImage(image->scaled(400,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
    //ui->colorFrameRight->layout()->addWidget(imageR);

    //ui->verticalSlider->setValue(0);
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
            base->setGeometry(0,0,225,30);
            base->setLayout(baseLayout);




            QLabel* imageL = new QLabel();
            imageL->setGeometry(0,0,175,30);
            imageL->setPixmap(QPixmap::fromImage(image->scaled(175,30,Qt::IgnoreAspectRatio,Qt::FastTransformation)));
            baseLayout->addWidget(imageL);

            QLineEdit* classification = new QLineEdit(QString(std::to_string(depth).c_str()));
            classification->setGeometry(0,0,50,30);
            baseLayout->addWidget(classification);

            return base;

        }
    }
    return NULL;
}

void DriveWidget::on_addElectrode_clicked()
{

    std::string selection(ui->electrodeSelection->itemText(ui->electrodeSelection->currentIndex()).toLocal8Bit().constData());

    if(_electrodeFrames.find(selection) == _electrodeFrames.end()){

        QVBoxLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0,0,0,0);

        QFrame *frame = new QFrame();
        frame->setFrameStyle(QFrame::Sunken | QFrame::Box);
        frame->setGeometry(0,0,225,30);
        frame->setLayout(layout);
        frame->setMaximumWidth(230);

        QLabel* textLabel = new QLabel(selection.c_str());
        QFont font("Arial",10,QFont::Bold);
        textLabel->setFont(font);
        layout->addWidget(textLabel);

        for(int i = -10; i <= 4;++i){
            QFrame *dataEntry = createElectordeImageEntry(selection,i);
            layout->addWidget(dataEntry);
        }

        ui->dataPanel->layout()->addWidget(frame);
        _electrodeFrames.insert(std::pair<std::string,QFrame*>(selection,frame));

    }
}

void DriveWidget::on_removeButton_clicked()
{
    std::string selection(ui->electrodeSelection->itemText(ui->electrodeSelection->currentIndex()).toLocal8Bit().constData());

    if(_electrodeFrames.find(selection) != _electrodeFrames.end()){
        QFrame* frame = _electrodeFrames.find(selection)->second;

        //find better solution, not sure if this will delete all children
        frame->close();
        ui->dataPanel->layout()->removeWidget(_electrodeFrames.find(selection)->second);

        _electrodeFrames.erase(selection);
        delete frame;
    }
}

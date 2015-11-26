#include "drivewidget.h"
#include "ui_drivewidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

DriveWidget::DriveWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    _data(data),
    ui(new Ui::DriveWidget),
    _latestStatus(0),
    _imageSetting(ImageSetting::FFT),
    _forceUpdate(false)
{
    ui->setupUi(this);
    this->setFloating(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(500);

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

void DriveWidget::addTrajectory(std::string name, std::shared_ptr<iElectrode> electrode){

}
void DriveWidget::updateTrajectory(std::string name){

}

void DriveWidget::addImageEntry(std::string name, int depth){

}

void DriveWidget::on_addElectrode_clicked()
{

    std::string selection(ui->electrodeSelection->itemText(ui->electrodeSelection->currentIndex()).toLocal8Bit().constData());

    if(_electrodeFrames.find(selection) == _electrodeFrames.end()){

        ElectrodeBaseFrame *frame = new ElectrodeBaseFrame(selection,this);

        frame->createFrameEntrys(_data,_imageSetting);

        ui->dataPanel->layout()->addWidget(frame);
        _electrodeFrames.insert(std::pair<std::string,ElectrodeBaseFrame*>(selection,frame));
        _data->incrementStatus();
    }
    this->resize(this->minimumWidth(),this->minimumHeight());
}

void DriveWidget::on_removeButton_clicked()
{
        std::string selection(ui->electrodeSelection->itemText(ui->electrodeSelection->currentIndex()).toLocal8Bit().constData());

        if(_electrodeFrames.find(selection) != _electrodeFrames.end()){
            ElectrodeBaseFrame* frame = _electrodeFrames.find(selection)->second;

            if(ElectrodeManager::getInstance().getTrackedElectrode() == selection){
                ElectrodeManager::getInstance().setTrackedElectrode("none");
            }

            //find better solution, not sure if this will delete all children
            frame->close();
            ui->dataPanel->layout()->removeWidget(_electrodeFrames.find(selection)->second);

            _electrodeFrames.erase(selection);
            delete frame;

            _data->incrementStatus();
        }
        this->resize(this->minimumWidth(),this->minimumHeight());
}


void DriveWidget::update(){
    uint64_t curStatus = _data->getDataSetStatus();

    for(auto it = _electrodeFrames.begin(); it != _electrodeFrames.end();it++){
        ElectrodeBaseFrame* frame = it->second;
        frame->checkToDisableRadioButton();

        if(frame->isCheckedForTracking()){
            Core::Math::Vec2d range = ElectrodeManager::getInstance().getElectrode(frame->electrodeName())->getDepthRange();
            int curPos = _data->getDisplayedDriveRange().y;
            if(curPos > range.y){
                ui->depthSelector->setSliderPosition(range.y);
                _data->setDisplayedDriveRange(Vec2i(range.x,range.y));
            }
            ui->depthSelector->setRange((int)range.x,(int)range.y);
        }
    }

    if(_latestStatus < curStatus || _forceUpdate){
        _latestStatus = curStatus;

        //iterate over all electrodes displayed
        for(auto it = _electrodeFrames.begin(); it != _electrodeFrames.end();it++){
            //add a check if recreation is needed!
            ElectrodeBaseFrame* frame = it->second;
            if(frame->childCount() < _data->getElectrode(it->first)->getDepthRange().x){

            }
            frame->resetFrame();

            frame->createFrameEntrys(_data,_imageSetting);
        }
        _forceUpdate = false;
    }

    this->resize(this->minimumWidth(),this->minimumHeight());
}

void DriveWidget::on_sginalButton_clicked()
{
    _imageSetting = ImageSetting::Signal;
    _forceUpdate = true;
    update();
}

void DriveWidget::on_probabilityButton_clicked()
{
    _imageSetting = ImageSetting::probability;
    _forceUpdate = true;
    update();
}

void DriveWidget::on_fftButton_clicked()
{
    _imageSetting = ImageSetting::FFT;
    _forceUpdate = true;
    update();
}

void DriveWidget::on_depthSelector_sliderMoved(int position)
{
    _data->setDisplayedDriveRange(Vec2i(-10,position));
}

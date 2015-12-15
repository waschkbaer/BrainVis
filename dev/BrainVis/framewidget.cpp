#include "framewidget.h"
#include "ui_framewidget.h"

#include <BrainVisIO/DataHandle.h>

#include <mainwindow.h>
#include <renderwidget.h>

FrameWidget::FrameWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    ui(new Ui::FrameWidget),
    _data(data)
{
    ui->setupUi(this);
    this->setFloating(true);
    this->show();

    if(data != nullptr){
        setSliders();
    }
}

FrameWidget::~FrameWidget()
{
    delete ui;
}

void FrameWidget::on_leftActive_clicked()
{
setSliders();
}

void FrameWidget::on_rightActive_clicked()
{
setSliders();
}

void FrameWidget::on_mLR_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curCenter = _data->getLeftFBBCenter();
        curCenter.x = (position/1000.0f);
        _data->setLeftFBBCenter(curCenter);
    }else{
        Vec3f curCenter = _data->getRightFBBCenter();
        curCenter.x = (position/1000.0f);
        _data->setRightFBBCenter(curCenter);
    }
}

void FrameWidget::on_mFB_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curCenter = _data->getLeftFBBCenter();
        curCenter.y = (position/1000.0f);
        _data->setLeftFBBCenter(curCenter);
    }else{
        Vec3f curCenter = _data->getRightFBBCenter();
        curCenter.y = (position/1000.0f);
        _data->setRightFBBCenter(curCenter);
    }
}

void FrameWidget::on_mUD_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curCenter = _data->getLeftFBBCenter();
        curCenter.z = (position/1000.0f);
        _data->setLeftFBBCenter(curCenter);
    }else{
        Vec3f curCenter = _data->getRightFBBCenter();
        curCenter.z = (position/1000.0f);
        _data->setRightFBBCenter(curCenter);
    }
}

void FrameWidget::on_sUD_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curScale = _data->getLeftFBBScale();
        curScale.z = (position/500.0f);
        _data->setLeftFBBScale(curScale);
    }else{
        Vec3f curScale = _data->getRightFBBScale();
        curScale.z = (position/500.0f);
        _data->setRightFBBScale(curScale);
    }
}

void FrameWidget::on_sLR_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curScale = _data->getLeftFBBScale();
        curScale.x = (position/500.0f);
        _data->setLeftFBBScale(curScale);
    }else{
        Vec3f curScale = _data->getRightFBBScale();
        curScale.x = (position/500.0f);
        _data->setRightFBBScale(curScale);
    }
}

void FrameWidget::on_sFB_sliderMoved(int position)
{
    if(ui->leftActive->isChecked()){
        Vec3f curScale = _data->getLeftFBBScale();
        curScale.y = (position/500.0f);
        _data->setLeftFBBScale(curScale);
    }else{
        Vec3f curScale = _data->getRightFBBScale();
        curScale.y = (position/500.0f);
        _data->setRightFBBScale(curScale);
    }
}

void FrameWidget::setSliders(){
    if(ui->leftActive->isChecked()){

        ui->mFB->setValue(_data->getLeftFBBCenter().y*1000);
        ui->mLR->setValue(_data->getLeftFBBCenter().x*1000);
        ui->mUD->setValue(_data->getLeftFBBCenter().y*1000);

        ui->sFB->setValue(_data->getLeftFBBCenter().y*500);
        ui->sLR->setValue(_data->getLeftFBBCenter().x*500);
        ui->sUD->setValue(_data->getLeftFBBCenter().y*500);

    }else{
        ui->mFB->setValue(_data->getRightFBBCenter().y*1000);
        ui->mLR->setValue(_data->getRightFBBCenter().x*1000);
        ui->mUD->setValue(_data->getRightFBBCenter().y*1000);

        ui->sFB->setValue(_data->getRightFBBCenter().y*500);
        ui->sLR->setValue(_data->getRightFBBCenter().x*500);
        ui->sUD->setValue(_data->getRightFBBCenter().y*500);

    }
}

void FrameWidget::on_pushButton_clicked()
{
    MainWindow* parent = (MainWindow*)this->parent();
    parent->getWorkingRenderer()->startFrameFind();
}

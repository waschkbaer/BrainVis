#include "renderwidget.h"
#include "ui_renderwidget.h"
#include "openglwidget.h"

#include "ModiSingleton.h"
#include "uis/mainwindow.h"


#include <io/DataHandle.h>
#include <renderer/DICOMRenderManager.h>
#include "ActivityManager.h"

using namespace BrainVis;

RenderWidget::RenderWidget(std::shared_ptr<DataHandle> data,
                           QWidget *parent,
                           int renderID,
                           RenderMode mode,
                           Core::Math::Vec2ui windowPosition) :
    QDockWidget(parent),
    ui(new Ui::RenderWidget),
    _data(data),
    _renderID(renderID),
    _isValid(true)
{
    //setFloating(true);
    ui->setupUi(this);
    move(windowPosition.x,windowPosition.y);

    show();

    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);

    ui->openGLWidget->setDataHandle(data);
    ui->openGLWidget->setRendererID(_renderID);


    DicomRenderManager::getInstance().getRenderer(renderID)->SetRenderMode(mode);
    switch(mode){
        case RenderMode::ThreeDMode :  ui->ThreeD->setEnabled(false);this->setWindowTitle("3d View");break;
        case RenderMode::XAxis      :  ui->XAxis->setEnabled(false);this->setWindowTitle("Sagittal View");break;
        case RenderMode::YAxis      :  ui->YAxis->setEnabled(false);this->setWindowTitle("Coronal View");break;
        case RenderMode::ZAxis      :  ui->ZAxis->setEnabled(false);this->setWindowTitle("Axial View");break;
    }
}

RenderWidget::~RenderWidget()
{
    delete ui;
}

void RenderWidget::Cleanup(){
    ui->openGLWidget->Cleanup();
    _isValid = false;
}

void RenderWidget::on_ThreeD_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::ThreeDMode);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(false);
    this->setWindowTitle("3d View");
}

void RenderWidget::on_ZAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::ZAxis);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(false);
    ui->ThreeD->setEnabled(true);
    this->setWindowTitle("Axial View");
}

void RenderWidget::on_XAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::XAxis);
    ui->XAxis->setEnabled(false);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);
    this->setWindowTitle("Sagittal View");
}

void RenderWidget::on_YAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::YAxis);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(false);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);
    this->setWindowTitle("Coronal View");


}
bool RenderWidget::isValid() const
{
    return _isValid;
}

int RenderWidget::renderID() const
{
    return _renderID;
}


void RenderWidget::keyPressEvent(QKeyEvent *event){
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    if(event->key() == Qt::Key_Alt){
        ui->openGLWidget->switchScrollMode();
    }
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Alt){
        ui->openGLWidget->switchScrollMode();
    }
}

void RenderWidget::setClipMode(DICOMClipMode mode){
    ui->openGLWidget->setClipMode(mode);
}

void RenderWidget::closeEvent(QCloseEvent *bar){
    DicomRenderManager::getInstance().deleteRenderer(_renderID);

    MainWindow* w = (MainWindow*)this->parent();
    if(w != NULL){
        w->removeRenderer(_renderID);
    }
}

#include "renderwidget.h"
#include "ui_renderwidget.h"
#include <openglwidget.h>

#include "ModiSingleton.h"
#include "mainwindow.h"


#include <BrainVisIO/DataHandle.h>
#include <renderer/DICOMRenderer/DICOMRenderManager.h>
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
        case RenderMode::ThreeDMode :  ui->ThreeD->setEnabled(false);break;
        case RenderMode::XAxis      :  ui->XAxis->setEnabled(false);break;
        case RenderMode::YAxis      :  ui->YAxis->setEnabled(false);break;
        case RenderMode::ZAxis      :  ui->ZAxis->setEnabled(false);break;
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
}

void RenderWidget::on_ZAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::ZAxis);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(false);
    ui->ThreeD->setEnabled(true);
}

void RenderWidget::on_XAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::XAxis);
    ui->XAxis->setEnabled(false);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);
}

void RenderWidget::on_YAxis_clicked()
{
    ActivityManager::getInstance().setActiveRenderer(_renderID);
    ui->openGLWidget->setRenderMode(RenderMode::YAxis);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(false);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);
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
    if(event->key() == Qt::Key_R){
        ModiSingleton::getInstance().setActiveModeLeftClick(Mode::CameraRotation);
    }
    if(event->key() == Qt::Key_M){
        ModiSingleton::getInstance().setActiveModeLeftClick(Mode::CameraMovement);
    }
    if(event->key() == Qt::Key_T){
        std::cout << "mode: tfeditor" << std::endl;
        ModiSingleton::getInstance().setActiveModeRightClick(Mode::TFEditor);
    }
    if(event->key() == Qt::Key_P){
        std::cout << "mode: picking" << std::endl;
        ModiSingleton::getInstance().setActiveModeRightClick(Mode::VolumePicking);
    }

    //manual move
    if(event->key() == Qt::Key_1){
        Vec3f o = _data->getMROffset();
        o.z -= 0.1f;
        _data->setMROffset(o);
    }
    if(event->key() == Qt::Key_2){
        Vec3f o = _data->getMROffset();
        o.z += 0.1f;
        _data->setMROffset(o);
    }

    if(event->key() == Qt::Key_3){
        Vec3f o = _data->getMROffset();
        o.x -= 0.1f;
        _data->setMROffset(o);
    }
    if(event->key() == Qt::Key_4){
        Vec3f o = _data->getMROffset();
        o.x += 0.1f;
        _data->setMROffset(o);
    }

    if(event->key() == Qt::Key_5){
        Vec3f o = _data->getMROffset();
        o.y -= 0.1f;
        _data->setMROffset(o);
    }
    if(event->key() == Qt::Key_6){
        Vec3f o = _data->getMROffset();
        o.y += 0.1f;
        _data->setMROffset(o);
    }

    //manual rot
    if(event->key() == Qt::Key_Y){
        Vec3f o = _data->getMRRotation();
        o.z -= 0.01f;
        _data->setMRRotation(o);
    }
    if(event->key() == Qt::Key_X){
        Vec3f o = _data->getMRRotation();
        o.z += 0.01f;
        _data->setMRRotation(o);
    }

    if(event->key() == Qt::Key_C){
        Vec3f o = _data->getMRRotation();
        o.x -= 0.01f;
        _data->setMRRotation(o);
    }
    if(event->key() == Qt::Key_V){
        Vec3f o = _data->getMRRotation();
        o.x += 0.01f;
        _data->setMRRotation(o);
    }

    if(event->key() == Qt::Key_B){
        Vec3f o = _data->getMRRotation();
        o.y -= 0.01f;
        _data->setMRRotation(o);
    }
    if(event->key() == Qt::Key_N){
        Vec3f o = _data->getMRRotation();
        o.y += 0.01f;
        _data->setMRRotation(o);
    }
    if(event->key() == Qt::Key_A){
        _data->setMRRotation(Vec3f(0,0,0));
    }
    if(event->key() == Qt::Key_Q){
        _data->setMROffset(Vec3f(0,0,0));
    }
    if(event->key() == Qt::Key_W){
        _data->setMRCTBlend(_data->getMRCTBlend()+0.05f);
    }
    if(event->key() == Qt::Key_S){
        _data->setMRCTBlend(_data->getMRCTBlend()-0.05f);
    }
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

#include "renderwidget.h"
#include "ui_renderwidget.h"
#include <openglwidget.h>

#include "ModiSingleton.h"
#include "mainwindow.h"


#include <BrainVisIO/DataHandle.h>

RenderWidget::RenderWidget(std::shared_ptr<DataHandle> data, QWidget *parent, int renderID) :
    QDockWidget(parent),
    ui(new Ui::RenderWidget),
    _data(data),
    _renderID(renderID),
    _isValid(true)
{
    setFloating(true);
    ui->setupUi(this);
    move(0,0);

    show();

    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(false);

    ui->openGLWidget->setDataHandle(data);
    ui->openGLWidget->setRendererID(_renderID);

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
    ui->openGLWidget->setRenderMode(RenderMode::ThreeDMode);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(false);
}

void RenderWidget::on_ZAxis_clicked()
{
    ui->openGLWidget->setRenderMode(RenderMode::ZAxis);
    ui->XAxis->setEnabled(true);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(false);
    ui->ThreeD->setEnabled(true);
}

void RenderWidget::on_XAxis_clicked()
{
    ui->openGLWidget->setRenderMode(RenderMode::XAxis);
    ui->XAxis->setEnabled(false);
    ui->YAxis->setEnabled(true);
    ui->ZAxis->setEnabled(true);
    ui->ThreeD->setEnabled(true);
}

void RenderWidget::on_YAxis_clicked()
{
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
    if(event->key() == Qt::Key_R){
        std::cout << "mode: rotatecam" << std::endl;
        ModiSingleton::getInstance().setActiveMode(Mode::CameraRotation);
    }
    if(event->key() == Qt::Key_M){
        std::cout << "mode: movecam" << std::endl;
        ModiSingleton::getInstance().setActiveMode(Mode::CameraMovement);
    }
    if(event->key() == Qt::Key_T){
        std::cout << "mode: tfeditor" << std::endl;
        ModiSingleton::getInstance().setActiveMode(Mode::TFEditor);
    }
    if(event->key() == Qt::Key_P){
        std::cout << "mode: picking" << std::endl;
        ModiSingleton::getInstance().setActiveMode(Mode::VolumePicking);
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
    if(event->key() == Qt::Key_0){
        ui->openGLWidget->setDoGradientDescent(true);
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

void RenderWidget::startGradientDescent(){
    ui->openGLWidget->setDoGradientDescent(true);
}

void RenderWidget::closeEvent(QCloseEvent *bar){
    MainWindow* w = (MainWindow*)this->parent();
    if(w != NULL){
        w->removeRenderer(_renderID);
    }
}

void RenderWidget::startFrameFind(){
    ui->openGLWidget->startFrameFind();
}

#include <QTimer>

#include "ctregistrationwidget.h"
#include "ui_ctregistrationwidget.h"

#include "mainwindow.h"
#include "renderwidget.h"
#include <BrainVisIO/DataHandle.h>

#include "ActivityManager.h"
#include <renderer/DICOMRenderer/DICOMRenderManager.h>

using namespace BrainVis;

CtRegistrationWidget::CtRegistrationWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    ui(new Ui::CtRegistrationWidget),
    _data(data)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(240);


    //init data
    ui->currentTranslationX->setText(QString(std::to_string(_data->getMROffset().x).c_str()));
    ui->currentTranslationY->setText(QString(std::to_string(_data->getMROffset().y).c_str()));
    ui->currentTranslationZ->setText(QString(std::to_string(_data->getMROffset().z).c_str()));

    ui->currentRotationX->setText(QString(std::to_string(_data->getMRRotation().x).c_str()));
    ui->currentRotationY->setText(QString(std::to_string(_data->getMRRotation().y).c_str()));
    ui->currentRotationZ->setText(QString(std::to_string(_data->getMRRotation().z).c_str()));

    _data->setFTranslationStep(ui->translationStepSize->value()/100.0f);
    _data->setFTranslationStepScale(ui->translationDegSize->value()/100.0f);
    _data->setFRotationStep(ui->rotatioStepSize->value()/100.0f);
    _data->setFRotationStepScale(ui->rotatioDegSize->value()/100.0f);

    ui->translationStepCurrent->setText(QString(std::to_string(ui->translationStepSize->value()/100.0f).c_str()));
    ui->translationDegCurrent->setText(QString(std::to_string(ui->translationDegSize->value()/100.0f).c_str()));
    ui->rotatioStepCurrent->setText(QString(std::to_string(ui->rotatioStepSize->value()/100.0f).c_str()));
    ui->rotatioDegCurrent->setText(QString(std::to_string(ui->rotatioDegSize->value()/100.0f).c_str()));


    _translationStep = ui->translationStepSize->value()/100.0f;
    _translationScaling = ui->translationDegSize->value()/100.0f;
    _rotationStep = ui->rotatioStepSize->value()/100.0f;
    _rotationScaling = ui->rotatioDegSize->value()/100.0f;

    this->setFloating(true);
    this->show();
}

CtRegistrationWidget::~CtRegistrationWidget()
{
    delete ui;
}

void CtRegistrationWidget::on_resetButton_clicked()
{
    _data->setFTranslationStep(_translationStep);
    _data->setFTranslationStepScale(_translationScaling);
    _data->setFRotationStep(_rotationStep);
    _data->setFRotationStepScale(_rotationScaling);
    _data->setMRRotation(Vec3f(0,0,0));
    _data->setMROffset(Vec3f(0,0,0));
}

void CtRegistrationWidget::on_registerButton_clicked()
{
    _data->setFTranslationStep(_translationStep);
    _data->setFTranslationStepScale(_translationScaling);
    _data->setFRotationStep(_rotationStep);
    _data->setFRotationStepScale(_rotationScaling);

    uint16_t handle = ActivityManager::getInstance().getActiveRenderer();
    std::shared_ptr<DICOMRenderer> r = DicomRenderManager::getInstance().getRenderer(handle);
    if(r != nullptr)
        r->setDoesGradientDescent(true);
}

void CtRegistrationWidget::on_translationStepSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->translationStepCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFTranslationStep(value);
    _translationStep = value;
}

void CtRegistrationWidget::on_translationDegSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->translationDegCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFTranslationStepScale(value);
    _translationScaling = value;
}

void CtRegistrationWidget::on_rotatioStepSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->rotatioStepCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFRotationStep(value);
    _rotationStep = value;
}

void CtRegistrationWidget::on_rotatioDegSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->rotatioDegCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFRotationStepScale(value);
    _rotationScaling = value;
}

void CtRegistrationWidget::update(){
    ui->currentTranslationX->setText(QString(std::to_string(_data->getMROffset().x).c_str()));
    ui->currentTranslationY->setText(QString(std::to_string(_data->getMROffset().y).c_str()));
    ui->currentTranslationZ->setText(QString(std::to_string(_data->getMROffset().z).c_str()));

    ui->currentRotationX->setText(QString(std::to_string(_data->getMRRotation().x).c_str()));
    ui->currentRotationY->setText(QString(std::to_string(_data->getMRRotation().y).c_str()));
    ui->currentRotationZ->setText(QString(std::to_string(_data->getMRRotation().z).c_str()));
}

void CtRegistrationWidget::closeEvent(QCloseEvent *bar){
    MainWindow* w = (MainWindow*)this->parent();
    if(w != NULL){
        w->closeRegistrationWidget();
    }
}

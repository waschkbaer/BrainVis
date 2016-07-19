#include <QTimer>

#include "ctregistrationwidget.h"
#include "ui_ctregistrationwidget.h"

#include "uis/mainwindow.h"
#include "uis/widgets/renderwidget.h"
#include <io/DataHandle.h>

#include "ActivityManager.h"
#include <renderer/DICOMRenderManager.h>
#include <stdlib.h>

using namespace BrainVis;

CtRegistrationWidget::CtRegistrationWidget(QWidget *parent, std::shared_ptr<DataHandle> data) :
    QDockWidget(parent),
    ui(new Ui::CtRegistrationWidget),
    _data(data),
    _registrationRenderer(nullptr),
    _fusionMode(FusionMode::OpenGL),
    _translation(_data->getMROffset()),
    _rotation(_data->getMRRotation())
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
    _registrationRenderer = DicomRenderManager::getInstance().getRenderer(RenderMode::XAxis);
    if(_registrationRenderer == nullptr)
        _registrationRenderer = DicomRenderManager::getInstance().getRenderer(handle);
    if(_registrationRenderer != nullptr)
        _registrationRenderer->setDoesGradientDescent(true,_fusionMode);
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
    if(_registrationRenderer != nullptr && _registrationRenderer->doesGradientDescent()){
        ui->registerButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
        this->setWindowTitle(QString("Registration Widget : Registering"));

    }else{
        ui->registerButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
        this->setWindowTitle(QString("Registration Widget"));
    }

    if(_translation != _data->getMROffset() || _rotation != _data->getMRRotation()){
        ui->currentTranslationX->setText(QString(std::to_string(_data->getMROffset().x).c_str()));
        ui->currentTranslationY->setText(QString(std::to_string(_data->getMROffset().y).c_str()));
        ui->currentTranslationZ->setText(QString(std::to_string(_data->getMROffset().z).c_str()));

        ui->currentRotationX->setText(QString(std::to_string(_data->getMRRotation().x).c_str()));
        ui->currentRotationY->setText(QString(std::to_string(_data->getMRRotation().y).c_str()));
        ui->currentRotationZ->setText(QString(std::to_string(_data->getMRRotation().z).c_str()));
        _translation = _data->getMROffset();
        _rotation = _data->getMRRotation();
    }
}

void CtRegistrationWidget::closeEvent(QCloseEvent *bar){
    MainWindow* w = (MainWindow*)this->parent();
    if(w != NULL){
        w->closeRegistrationWidget();
    }
}

void CtRegistrationWidget::on_glButton_clicked()
{
_fusionMode = FusionMode::OpenGL;
}

void CtRegistrationWidget::on_cudaButton_clicked()
{
_fusionMode = FusionMode::CUDA;
}

void CtRegistrationWidget::on_cpuButton_clicked()
{
_fusionMode = FusionMode::CPU;
}

void CtRegistrationWidget::on_currentTranslationX_editingFinished()
{
    _translation.x = ui->currentTranslationX->text().toFloat();
    _data->setMROffset(_translation);
}

void CtRegistrationWidget::on_currentTranslationY_editingFinished()
{
    _translation.y = ui->currentTranslationY->text().toFloat();
    _data->setMROffset(_translation);
}

void CtRegistrationWidget::on_currentTranslationZ_editingFinished()
{
    _translation.z = ui->currentTranslationZ->text().toFloat();
    _data->setMROffset(_translation);
}

void CtRegistrationWidget::on_currentTranslationX_returnPressed()
{
    on_currentTranslationX_editingFinished();
}

void CtRegistrationWidget::on_currentTranslationY_returnPressed()
{
    on_currentTranslationY_editingFinished();
}

void CtRegistrationWidget::on_currentTranslationZ_returnPressed()
{
    on_currentTranslationZ_editingFinished();
}

void CtRegistrationWidget::on_currentRotationX_editingFinished()
{
    _rotation.x = ui->currentRotationX->text().toFloat();
    _data->setMRRotation(_rotation);
}

void CtRegistrationWidget::on_currentRotationY_editingFinished()
{
    _rotation.y = ui->currentRotationY->text().toFloat();
    _data->setMRRotation(_rotation);
}

void CtRegistrationWidget::on_currentRotationZ_editingFinished()
{
    _rotation.z = ui->currentRotationZ->text().toFloat();
    _data->setMRRotation(_rotation);
}

void CtRegistrationWidget::on_currentRotationX_returnPressed()
{
    on_currentRotationX_editingFinished();
}

void CtRegistrationWidget::on_currentRotationY_returnPressed()
{
    on_currentRotationY_editingFinished();
}

void CtRegistrationWidget::on_currentRotationZ_returnPressed()
{
    on_currentRotationZ_editingFinished();
}

void CtRegistrationWidget::on_randombutton_clicked()
{
    /* initialize random seed: */
     srand (time(NULL));

     /* generate random translation between -20 and + 20: */
     _translation.x = (rand() % 40 + 1)-20;
     _translation.y = (rand() % 40 + 1)-20;
     _translation.z = (rand() % 40 + 1)-20;

     /* generate random translation between -0.50 and + 0.50: */
     _rotation.x = ((float)(rand() % 100 + 1)-50.0f)/100.0f;
     _rotation.y = ((float)(rand() % 100 + 1)-50.0f)/100.0f;
     _rotation.z = ((float)(rand() % 100 + 1)-50.0f)/100.0f;

     _data->setMROffset(_translation);
     _data->setMRRotation(_rotation);
}

#include "ctregistrationwidget.h"
#include "ui_ctregistrationwidget.h"

#include <mainwindow.h>
#include <QTimer>

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

    this->setFloating(true);
    this->show();
}

CtRegistrationWidget::~CtRegistrationWidget()
{
    delete ui;
}

void CtRegistrationWidget::on_resetButton_clicked()
{
    _data->setMRRotation(Vec3f(0,0,0));
    _data->setMROffset(Vec3f(0,0,0));
}

void CtRegistrationWidget::on_registerButton_clicked()
{
    MainWindow* parent = (MainWindow*)this->parent();
    parent->getWorkingRenderer()->startGradientDescent();
}

void CtRegistrationWidget::on_translationStepSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->translationStepCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFTranslationStep(value);
}

void CtRegistrationWidget::on_translationDegSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->translationDegCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFTranslationStepScale(value);
}

void CtRegistrationWidget::on_rotatioStepSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->rotatioStepCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFRotationStep(value);
}

void CtRegistrationWidget::on_rotatioDegSize_sliderMoved(int position)
{
    float value = (float)position/100.0f;
    ui->rotatioDegCurrent->setText(QString(std::to_string(value).c_str()));
    _data->setFRotationStepScale(value);
}

void CtRegistrationWidget::update(){
    ui->currentTranslationX->setText(QString(std::to_string(_data->getMROffset().x).c_str()));
    ui->currentTranslationY->setText(QString(std::to_string(_data->getMROffset().y).c_str()));
    ui->currentTranslationZ->setText(QString(std::to_string(_data->getMROffset().z).c_str()));

    ui->currentRotationX->setText(QString(std::to_string(_data->getMRRotation().x).c_str()));
    ui->currentRotationY->setText(QString(std::to_string(_data->getMRRotation().y).c_str()));
    ui->currentRotationZ->setText(QString(std::to_string(_data->getMRRotation().z).c_str()));
}

#include "meroptions.h"
#include "ui_meroptions.h"

#include <iostream>

MEROptions::MEROptions(std::shared_ptr<MERRecordSettings> settings, QWidget *parent) :
    QDockWidget(parent),
    _settings(settings),
    ui(new Ui::MEROptions)
{
    ui->setupUi(this);

    ui->ex->setText(QString(std::to_string(settings->_entryPosition.x).c_str()));
    ui->ey->setText(QString(std::to_string(settings->_entryPosition.y).c_str()));
    ui->ez->setText(QString(std::to_string(settings->_entryPosition.z).c_str()));

    ui->tx->setText(QString(std::to_string(settings->_targetPosition.x).c_str()));
    ui->ty->setText(QString(std::to_string(settings->_targetPosition.y).c_str()));
    ui->tz->setText(QString(std::to_string(settings->_targetPosition.z).c_str()));

    if(settings->_isRightSide){
        ui->radioRight->setChecked(true);
        ui->radioLeft->setChecked(false);
    }else{
        ui->radioLeft->setChecked(true);
        ui->radioRight->setChecked(false);
    }

    if(settings->_centerIndex > 1){
        ui->cenCheck->setChecked(true);
        ui->cenSpin->setValue(settings->_centerIndex-1);
    }else{
         ui->cenCheck->setChecked(false);
         ui->cenSpin->setValue(5);
    }


    if(settings->_lateralIndex > 1){
        ui->latCheck->setChecked(true);
        ui->latSpin->setValue(settings->_lateralIndex-1);
    }else{
         ui->latCheck->setChecked(false);
         ui->latSpin->setValue(5);
    }



    if(settings->_anteriorIndex > 1){
        ui->antCheck->setChecked(true);
        ui->antSpin->setValue(settings->_anteriorIndex-1);
    }else{
         ui->antCheck->setChecked(false);
         ui->antSpin->setValue(5);
    }



    if(settings->_posteriorIndex > 1){
        ui->posCheck->setChecked(true);
        ui->posSpin->setValue(settings->_posteriorIndex-1);
    }else{
         ui->posCheck->setChecked(false);
         ui->posSpin->setValue(5);
    }



    if(settings->_medianIndex > 1){
        ui->medCheck->setChecked(true);
        ui->medSpin->setValue(settings->_medianIndex-1);
    }else{
         ui->medCheck->setChecked(false);
         ui->medSpin->setValue(5);
    }

    this->show();
}

MEROptions::~MEROptions()
{
    delete ui;
}


void MEROptions::updateSettings(){
    _settings->_entryPosition.x = ui->ex->text().toFloat();
    _settings->_entryPosition.y = ui->ey->text().toFloat();
    _settings->_entryPosition.z = ui->ez->text().toFloat();

    _settings->_targetPosition.x = ui->tx->text().toFloat();
    _settings->_targetPosition.y = ui->ty->text().toFloat();
    _settings->_targetPosition.z = ui->tz->text().toFloat();


    if(ui->cenCheck->isChecked()){
        _settings->_centerIndex =  ui->cenSpin->value()+1;
    }else{
        _settings->_centerIndex = -1;
    }

    if(ui->latCheck->isChecked()){
        _settings->_lateralIndex =  ui->latSpin->value()+1;
    }else{
        _settings->_lateralIndex = -1;
    }

    if(ui->antCheck->isChecked()){
        _settings->_anteriorIndex =  ui->antSpin->value()+1;
    }else{
        _settings->_anteriorIndex = -1;
    }

    if(ui->posCheck->isChecked()){
        _settings->_posteriorIndex =  ui->posSpin->value()+1;
    }else{
        _settings->_posteriorIndex = -1;
    }

    if(ui->medCheck->isChecked()){
        _settings->_medianIndex =  ui->medSpin->value()+1;
    }else{
        _settings->_medianIndex = -1;
    }

    if(ui->radioRight->isChecked())
        _settings->_isRightSide = true;
    else
        _settings->_isRightSide = false;

}

void MEROptions::on_saveButton_clicked()
{
    updateSettings();
}

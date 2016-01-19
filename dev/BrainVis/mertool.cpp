#include "mertool.h"
#include "ui_mertool.h"

#include "merwidgets/merimageentry.h"

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>

#include "merwidgets/merelectrodeentry.h"

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);

    setFloating(true);

    show();


    std::string path = "C:/Users/andre/Documents/BrainVis/build/bin/13_1_2016_m2_rechts";
    std::shared_ptr<BrainVisIO::MERData::MERBundle> right = BrainVisIO::MERData::MERFileManager::getInstance().openFolder(path);

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = right->getElectrode("lat");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant = right->getElectrode("ant");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen = right->getElectrode("cen");

    merelectrodeentry* electrodeEntry;


    electrodeEntry = new merelectrodeentry(this);
    ui->dataFrame->layout()->addWidget(electrodeEntry);
    electrodeEntry->createElectrodeEntries(cen);

    electrodeEntry = new merelectrodeentry(this);
    ui->dataFrame->layout()->addWidget(electrodeEntry);
    electrodeEntry->createElectrodeEntries(lat);

    electrodeEntry = new merelectrodeentry(this);
    ui->dataFrame->layout()->addWidget(electrodeEntry);
    electrodeEntry->createElectrodeEntries(ant);
}

void MERTool::update(){

}

MERTool::~MERTool()
{
    delete ui;
}

void MERTool::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked()){
        ui->liveModeFrame->show();
    }else{
        ui->liveModeFrame->hide();
    }
}

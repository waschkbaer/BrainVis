#include "mertool.h"
#include "ui_mertool.h"

#include "merwidgets/merimageentry.h"

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    std::string path = "C:/Users/andre/Documents/BrainVis/build/bin/13_1_2016_m2_rechts";
    std::shared_ptr<BrainVisIO::MERData::MERBundle> right = BrainVisIO::MERData::MERFileManager::getInstance().openFolder(path);

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = right->getElectrode("ant");

    MERimageentry* imgentry = NULL;

    for(int i = -10; i <= 5; ++i){
        imgentry = new MERimageentry(this);
        imgentry->createSpectralImage(lat->getMERData(i)->getSpectralPowerNormalizedAndWindowed());
        ui->dataFrame->layout()->addWidget(imgentry);
    }


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

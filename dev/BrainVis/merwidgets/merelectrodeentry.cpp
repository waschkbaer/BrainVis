#include "merelectrodeentry.h"
#include "ui_merelectrodeentry.h"


#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>

#include "merimageentry.h"

merelectrodeentry::merelectrodeentry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::merelectrodeentry)
{
    ui->setupUi(this);

      show();
}

merelectrodeentry::~merelectrodeentry()
{
    delete ui;
}

void merelectrodeentry::createElectrodeEntries(std::shared_ptr<BrainVisIO::MERData::MERElectrode> electrode){

    MERimageentry* imgentry = NULL;

    for(int i = -10; i <= 5; ++i){
        imgentry = new MERimageentry(this);
        imgentry->createSpectralImage(electrode->getMERData(i)->getSpectralPowerNormalizedAndWindowed());
        ui->entryframe->layout()->addWidget(imgentry);
    }
}

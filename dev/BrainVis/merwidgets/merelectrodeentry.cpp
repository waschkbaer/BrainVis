#include "merelectrodeentry.h"
#include "ui_merelectrodeentry.h"


#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>

#include "merimageentry.h"

merelectrodeentry::merelectrodeentry(const std::string& electrodeName, QWidget *parent) :
    QWidget(parent),
    _electrodeName(electrodeName),
    ui(new Ui::merelectrodeentry)
{
    ui->setupUi(this);
    show();

    ui->nameLabel->setText(QString(electrodeName.c_str()));
}

merelectrodeentry::~merelectrodeentry()
{
    delete ui;
}

void merelectrodeentry::createElectrodeEntries(std::shared_ptr<BrainVisIO::MERData::MERElectrode> electrode, MERDisplayMode mode){

    MERimageentry* imgentry = NULL;

    for(int i = -10; i <= 5; ++i){
        std::shared_ptr<BrainVisIO::MERData::MERData> data = electrode->getMERData(i);
        if(data != nullptr){
            imgentry = new MERimageentry(i,_electrodeName,this);
            if(mode == MERDisplayMode::fft)
                imgentry->createSpectralImage(electrode->getMERData(i)->getSpectralPowerNormalizedAndWindowed());
            else if(mode == MERDisplayMode::signal)
                imgentry->createSignalImage(electrode->getMERData(i)->getSignalFiltered(5));
            ui->entryframe->layout()->addWidget(imgentry);
            _widgets.push_back(imgentry);
        }
    }
}

void merelectrodeentry::clean(){
    for(QWidget* w : _widgets){
        ui->entryframe->layout()->removeWidget(w);
        delete w;
    }
    _widgets.clear();

}

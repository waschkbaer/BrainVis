#include "merelectrodeentry.h"
#include "ui_merelectrodeentry.h"
#include "../mertool.h"

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>
#include <BrainVisIO/Data/MERBundleManager.h>
#include <BrainVisIO/DataHandleManager.h>

merelectrodeentry::merelectrodeentry(const std::string& electrodeName, MERTool *parent) :
    QWidget(parent),
    _electrodeName(electrodeName),
    _electrode(nullptr),
    ui(new Ui::merelectrodeentry),
    _widgets(),
    _parent(parent)
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
    _electrode = electrode;

    for(int i = -10; i <= 5; ++i){
        std::shared_ptr<BrainVisIO::MERData::MERData> data = electrode->getMERData(i);
        if(data != nullptr){
            imgentry = new MERimageentry(i,_electrodeName,data,this);
            if(mode == MERDisplayMode::fft)
                imgentry->createSpectralImage(electrode->getMERData(i)->getSpectralPowerNormalizedAndWindowed());
            else if(mode == MERDisplayMode::signal)
                imgentry->createSignalImage(electrode->getMERData(i)->getSignalFiltered(5));
            ui->entryframe->layout()->addWidget(imgentry);
            _widgets.push_back(imgentry);
        }
    }
}
void merelectrodeentry::disableSelection(){
    _parent->disableSelection();
}

void merelectrodeentry::disableAllImages(){
    for(MERimageentry* w : _widgets){
        w->setSelected(false);
    }
}

void merelectrodeentry::clean(){
    for(QWidget* w : _widgets){
        ui->entryframe->layout()->removeWidget(w);
        delete w;
    }
    _widgets.clear();

}

void merelectrodeentry::on_displayElectrode_clicked(bool checked)
{
    if(_electrode != nullptr){
        _electrode->setElectrodeVisible(checked);
        BrainVisIO::MERData::MERBundleManager::getInstance().incrementBundleStatus();
    }
}

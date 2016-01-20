#include "mertool.h"
#include "ui_mertool.h"

#include <QFileDialog>

#include "merwidgets/merimageentry.h"
#include "BrainVisIO/Data/MERBundleManager.h"

#include "merwidgets/merelectrodeentry.h"

#include <mocca/base/StringTools.h>

using namespace BrainVisIO::MERData;

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    _displayMode(MERDisplayMode::fft),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    on_checkBox_clicked();
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

void MERTool::on_loadButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString directory = dialog.getExistingDirectory();

    std::vector<std::string> p = mocca::splitString<std::string>(directory.toStdString(),'/');

    std::shared_ptr<BrainVisIO::MERData::MERBundle> bunlde = BrainVisIO::MERData::MERFileManager::getInstance().openFolder(directory.toStdString());

    MERBundleManager::getInstance().addMERBundle(p[p.size()-1], bunlde);

    if( ui->BundleSelection->findText(QString(p[p.size()-1].c_str())) == -1)
        ui->BundleSelection->addItem(QString(p[p.size()-1].c_str()));

    ui->BundleSelection->setCurrentIndex(ui->BundleSelection->findText(QString(p[p.size()-1].c_str())));
    on_BundleSelection_activated(ui->BundleSelection->currentText());
}

void MERTool::on_BundleSelection_activated(const QString &arg1)
{
    updateData(arg1.toStdString());
}

void MERTool::updateData(const std::string& bundlename){

    std::shared_ptr<MERBundle> bundle =  MERBundleManager::getInstance().getMERBundle(bundlename);
    if(bundle == nullptr) return;

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = bundle->getElectrode("lat");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant = bundle->getElectrode("ant");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen = bundle->getElectrode("cen");

    if(_electrodeEntries.find("lat") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("lat",std::make_shared<merelectrodeentry>("Lat-Right",this)));
    else
        _electrodeEntries.find("lat")->second->clean();

    if(_electrodeEntries.find("ant") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("ant",std::make_shared<merelectrodeentry>("Ant-Right",this)));
    else
        _electrodeEntries.find("ant")->second->clean();

    if(_electrodeEntries.find("cen") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("cen",std::make_shared<merelectrodeentry>("Cen-Right",this)));
    else
        _electrodeEntries.find("cen")->second->clean();

    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("cen")->second.get());
    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("lat")->second.get());
    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("ant")->second.get());

    _electrodeEntries.find("lat")->second->createElectrodeEntries(lat,_displayMode);
    _electrodeEntries.find("ant")->second->createElectrodeEntries(ant,_displayMode);
    _electrodeEntries.find("cen")->second->createElectrodeEntries(cen,_displayMode);
}

void MERTool::on_signalRadio_toggled(bool checked)
{
    if(checked){
        ui->fftRadio->setChecked(false);
        ui->classifierRadio->setChecked(false);
        _displayMode = MERDisplayMode::signal;
        updateData(ui->BundleSelection->currentText().toStdString());
    }
}

void MERTool::on_fftRadio_toggled(bool checked)
{
    if(checked){
        ui->signalRadio->setChecked(false);
        ui->classifierRadio->setChecked(false);
        _displayMode = MERDisplayMode::fft;
        updateData(ui->BundleSelection->currentText().toStdString());
    }
}

void MERTool::on_classifierRadio_toggled(bool checked)
{
    if(checked){
        ui->fftRadio->setChecked(false);
        ui->signalRadio->setChecked(false);
        _displayMode = MERDisplayMode::classifier;
        updateData(ui->BundleSelection->currentText().toStdString());
    }
}

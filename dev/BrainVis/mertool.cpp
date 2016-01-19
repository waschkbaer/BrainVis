#include "mertool.h"
#include "ui_mertool.h"

#include "merwidgets/merimageentry.h"



#include "merwidgets/merelectrodeentry.h"

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    on_checkBox_clicked();

    /*std::string path = "C:/Users/andre/Documents/BrainVis/build/bin/13_1_2016_m2_rechts";
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
    electrodeEntry->createElectrodeEntries(ant);*/
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
    std::string path = "C:/Users/andre/Documents/BrainVis/build/bin/13_1_2016_m2_rechts";
    std::shared_ptr<BrainVisIO::MERData::MERBundle> right = BrainVisIO::MERData::MERFileManager::getInstance().openFolder(path);

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = right->getElectrode("lat");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant = right->getElectrode("ant");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen = right->getElectrode("cen");

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

    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("lat")->second.get());
    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("ant")->second.get());
    ui->dataFrame->layout()->addWidget(_electrodeEntries.find("cen")->second.get());

    _electrodeEntries.find("lat")->second->createElectrodeEntries(lat);
    _electrodeEntries.find("ant")->second->createElectrodeEntries(ant);
    _electrodeEntries.find("cen")->second->createElectrodeEntries(cen);
}

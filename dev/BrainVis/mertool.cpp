#include "mertool.h"
#include "ui_mertool.h"

#include <QFileDialog>
#include <QTimer>

#include "merwidgets/merimageentry.h"
#include "BrainVisIO/Data/MERBundleManager.h"

#include "merwidgets/merelectrodeentry.h"

#include <mocca/base/StringTools.h>


using namespace BrainVisIO::MERData;

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    _displayMode(MERDisplayMode::fft),
    _fftThreadStop(false),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);

    setFloating(true);

    show();

    on_checkBox_clicked();

    timerId = startTimer(1000);
}

void MERTool::timerEvent(QTimerEvent *event)
{
    if(_MERClient != nullptr && ui->BundleSelection->currentIndex() >= 0 && _MERClient->getIsRecording()){
        updateData(ui->BundleSelection->currentText().toStdString());
    }
}

void MERTool::closeEvent(QCloseEvent *event){
    if(_MERClient != nullptr){
        _MERClient->setIsConnected(false);
        _MERClient->waitForThread();
        _MERClient.reset();
    }

    if(_fftCalcThread != nullptr){
        _fftThreadStop = true;
        _fftCalcThread->join();
        _fftCalcThread.reset();
    }
    killTimer(timerId);
}

MERTool::~MERTool()
{
    if(_MERClient != nullptr){
        _MERClient->setIsConnected(false);
        _MERClient->waitForThread();
        _MERClient.reset();
    }

    if(_fftCalcThread != nullptr){
        _fftThreadStop = true;
        _fftCalcThread->join();
        _fftCalcThread.reset();
    }
    killTimer(timerId);
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

    std::cout << "loaded"<<std::endl;

    MERBundleManager::getInstance().addMERBundle(p[p.size()-1], bunlde);

    if( ui->BundleSelection->findText(QString(p[p.size()-1].c_str())) == -1)
        ui->BundleSelection->addItem(QString(p[p.size()-1].c_str()));

    ui->BundleSelection->setCurrentIndex(ui->BundleSelection->findText(QString(p[p.size()-1].c_str())));
    on_BundleSelection_activated(ui->BundleSelection->currentText());
}

void MERTool::on_BundleSelection_activated(const QString &arg1)
{
    updateData(arg1.toStdString());
    MERBundleManager::getInstance().activateBundle(arg1.toStdString());
}

void MERTool::updateData(const std::string& bundlename){

    std::shared_ptr<MERBundle> bundle =  MERBundleManager::getInstance().getMERBundle(bundlename);
    if(bundle == nullptr) return;

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = bundle->getElectrode("lat");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant = bundle->getElectrode("ant");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen = bundle->getElectrode("cen");

    if(_electrodeEntries.find("lat") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("lat",std::make_shared<merelectrodeentry>("Lat-Right")));
    else
        _electrodeEntries.find("lat")->second->clean();

    if(_electrodeEntries.find("ant") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("ant",std::make_shared<merelectrodeentry>("Ant-Right")));
    else
        _electrodeEntries.find("ant")->second->clean();

    if(_electrodeEntries.find("cen") == _electrodeEntries.end())
        _electrodeEntries.insert(std::pair<std::string,std::shared_ptr<merelectrodeentry>>("cen",std::make_shared<merelectrodeentry>("Cen-Right")));
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

void MERTool::on_connectButton_clicked()
{
    if(_MERClient != nullptr ) return;
    std::shared_ptr<MERBundle> b = std::make_shared<MERBundle>();
    b->addElectrode("lat");
    b->addElectrode("cen");
    b->addElectrode("ant");

    MERBundleManager::getInstance().addMERBundle("recording",b);
    if( ui->BundleSelection->findText(QString("recording")) == -1)
        ui->BundleSelection->addItem(QString("recording"));

    //ui->BundleSelection->setCurrentIndex(ui->BundleSelection->findText(QString("recording")));
    //on_BundleSelection_activated(ui->BundleSelection->currentText());

    MERElectrodeIds ids(2,3,4);

    _MERClient = std::unique_ptr<MERClient>(new MERClient(400000));
    _MERClient->setCurrentBundle(b);
    _MERClient->setCurrentDepth(-10);

    _MERClient->connect(ui->hostEdit->text().toStdString(),ids);

    _MERClient->setIsRecording(false);

    _fftThreadStop = false;
    _fftCalcThread = std::unique_ptr<std::thread>(new std::thread(&MERTool::fftCalcThreadRun,this));
}

void MERTool::on_recordButton_clicked()
{
    if(_MERClient == nullptr) return;

    std::string infostring  = "Depth: "+ std::to_string(_MERClient->getCurrentDepth());
    if(_MERClient->getIsRecording()){
        _MERClient->setIsRecording(false);
        ui->recordButton->setText(QString("Start Rec"));
        infostring+= " NOT RECORDING";
        ui->infoText->setText(QString(infostring.c_str()));
    }else{
        _MERClient->setIsRecording(true);
        ui->recordButton->setText(QString("Stop Rec"));
        infostring+= " RECORDING";
        ui->infoText->setText(QString(infostring.c_str()));
    }
}

void MERTool::on_nextButton_clicked()
{
    if(_MERClient == nullptr ) return;

    std::shared_ptr<MERBundle> bundle = MERBundleManager::getInstance().getMERBundle(ui->BundleSelection->currentText().toStdString());
    if(bundle != nullptr){
        bundle->getElectrode("lat")->newRecording();
        bundle->getElectrode("ant")->newRecording();
        bundle->getElectrode("cen")->newRecording();
        _MERClient->setCurrentDepth(bundle->getElectrode("lat")->getLatestDepth());

        std::string infostring  = "Depth: "+ std::to_string(_MERClient->getCurrentDepth());
        if(_MERClient->getIsRecording()){
            infostring+= " RECORDING";
            ui->infoText->setText(QString(infostring.c_str()));
        }else{
            infostring+= " NOT RECORDING";
            ui->infoText->setText(QString(infostring.c_str()));
        }
    }
}

void MERTool::on_saveButton_clicked()
{

}

void MERTool::on_optionsButton_clicked()
{
    if(_currentRecordingSettings == nullptr)
        _currentRecordingSettings = std::make_shared<MERRecordSettings>();
    MEROptions* options = new MEROptions(_currentRecordingSettings);
}

void MERTool::fftCalcThreadRun(){
    std::shared_ptr<MERData> data;
    std::shared_ptr<MERBundle> bundle;
    int lastRecord = 0;
    int depth = _MERClient->getCurrentDepth();

    while(!_fftThreadStop){
        if(depth != _MERClient->getCurrentDepth()){
            depth = _MERClient->getCurrentDepth();
            lastRecord = 0;
        }

        if( _MERClient != nullptr &&
            _MERClient->getIsRecording() &&
            ui->checkBox->isChecked()){

            bundle = MERBundleManager::getInstance().getMERBundle(ui->BundleSelection->currentText().toStdString());

            if(bundle != nullptr){
                merClientMutex.lock();

                data = bundle->getElectrode("lat")->getMERData(_MERClient->getCurrentDepth());

                if(data->getRecordedSeconds() >= 5 && data->getRecordedSeconds()-lastRecord > 0){
                    data->executeFFTWelch(5,true);
                }

                data = bundle->getElectrode("ant")->getMERData(_MERClient->getCurrentDepth());
                if(data->getRecordedSeconds() >= 5 && data->getRecordedSeconds()-lastRecord > 0){
                    data->executeFFTWelch(5,true);
                }

                data = bundle->getElectrode("cen")->getMERData(_MERClient->getCurrentDepth());
                if(data->getRecordedSeconds() >= 5 && data->getRecordedSeconds()-lastRecord > 0){
                    data->executeFFTWelch(5,true);
                }
                if(data->getRecordedSeconds()-lastRecord > 0)
                    lastRecord = data->getRecordedSeconds();

                merClientMutex.unlock();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
}

void MERTool::on_disconnectButton_clicked()
{
    if(_MERClient != nullptr){
        _MERClient->setIsConnected(false);
        _MERClient->waitForThread();
        _MERClient.reset();
    }

    if(_fftCalcThread != nullptr){
        _fftThreadStop = true;
        _fftCalcThread->join();
        _fftCalcThread.reset();
    }
}

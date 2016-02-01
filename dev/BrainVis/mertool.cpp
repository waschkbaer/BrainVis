#include "mertool.h"
#include "ui_mertool.h"

#include "mainwindow.h"

#include <QFileDialog>
#include <QTimer>

#include "merwidgets/merimageentry.h"
#include "BrainVisIO/Data/MERBundleManager.h"
#include "ActivityManager.h"
#include "BrainVisIO/DataHandleManager.h"

#include "merwidgets/merelectrodeentry.h"

#include <mocca/base/StringTools.h>
#include <ctime>
#include <stdlib.h>

using namespace BrainVis;
using namespace BrainVisIO::MERData;

MERTool::MERTool(QWidget *parent) :
    QDockWidget(parent),
    _displayMode(MERDisplayMode::fft),
    _fftThreadStop(false),
    ui(new Ui::MERTool)
{
    ui->setupUi(this);
    //setFloating(true);
    show();

    on_checkBox_clicked();
    on_classificationBox_clicked();

    _perceptron = std::unique_ptr<Perceptron>(new Perceptron());

    timerId = startTimer(1000);

    loadDataBase();
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

    MainWindow* parent = (MainWindow*)this->parent();
    parent->closeMERTool();
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

void MERTool::loadDataBase(){
    std::vector<std::string> electrodes = MERBundleManager::getInstance().getRegisteredBundles();

    for(const std::string s: electrodes){
        if( ui->BundleSelection->findText(QString(s.c_str())) == -1)
            ui->BundleSelection->addItem(QString(s.c_str()));
    }

    if(electrodes.size() > 0){
        ui->BundleSelection->setCurrentIndex(0);
        on_BundleSelection_activated(ui->BundleSelection->currentText());
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

    updateSettings(bunlde);

    if( ui->BundleSelection->findText(QString(p[p.size()-1].c_str())) == -1)
        ui->BundleSelection->addItem(QString(p[p.size()-1].c_str()));

    ui->BundleSelection->setCurrentIndex(ui->BundleSelection->findText(QString(p[p.size()-1].c_str())));
    on_BundleSelection_activated(ui->BundleSelection->currentText());

    if( DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset()) != nullptr)
        DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset())->incrementStatus();
}

void MERTool::updateSettings(const std::shared_ptr<BrainVisIO::MERData::MERBundle> bundle){
    if(_currentRecordingSettings == nullptr){
        _currentRecordingSettings = std::make_shared<MERRecordSettings>();
    }

    _currentRecordingSettings->_targetPosition = bundle->getTarget();
    _currentRecordingSettings->_entryPosition = bundle->getEntry();
    _currentRecordingSettings->_isRightSide = bundle->getIsRightSide();
}

void MERTool::on_BundleSelection_activated(const QString &arg1)
{
    updateData(arg1.toStdString());
    updateSettings(MERBundleManager::getInstance().getMERBundle(arg1.toStdString()));
    MERBundleManager::getInstance().activateBundle(arg1.toStdString());

    if( DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset()) != nullptr)
        DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset())->incrementStatus();
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

    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    std::string dateString =    std::to_string(now->tm_mday)+"_"+
                                std::to_string(now->tm_mon+1)+"_"+
                                std::to_string(now->tm_yday+1989);

    MERBundleManager::getInstance().addMERBundle(dateString,b);
    if( ui->BundleSelection->findText(QString(dateString.c_str())) == -1)
        ui->BundleSelection->addItem(QString(dateString.c_str()));

    MERElectrodeIds ids(2,3,4);

    _MERClient = std::unique_ptr<MERClient>(new MERClient(400000));
    _MERClient->setCurrentBundle(b);
    _MERClient->setCurrentDepth(-10);

    _MERClient->connect(ui->hostEdit->text().toStdString(),ids);

    _MERClient->setIsRecording(false);

    //force options
    on_optionsButton_clicked();
    MERBundleManager::getInstance().activateBundle(dateString);
    ActivityManager::getInstance().setActiveElectrode(dateString);

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
    MERFileManager::getInstance().saveRecordings(MERBundleManager::getInstance().getMERBundle(ActivityManager::getInstance().getActiveElectrode()),
                                                 ActivityManager::getInstance().getActiveElectrode());
}

void MERTool::on_optionsButton_clicked()
{
    if(_currentRecordingSettings == nullptr)
        _currentRecordingSettings = std::make_shared<MERRecordSettings>();
    MEROptions* options = new MEROptions(_currentRecordingSettings,
                                         MERBundleManager::getInstance().
                                         getMERBundle(ui->BundleSelection->currentText().toStdString()));
}

void MERTool::fftCalcThreadRun(){
    std::shared_ptr<MERData> data;
    std::shared_ptr<MERBundle> bundle;
    int lastRecord = 0;
    int depth = _MERClient->getCurrentDepth();
    std::vector<double> perceptronInputs;

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
                    perceptronInputs = data->getSpectralPowerNormalizedAndWindowed();
                    perceptronInputs.push_back(_MERClient->getCurrentDepth());
                    _perceptron->setInputs(perceptronInputs);
                    _perceptron->classify();
                }

                data = bundle->getElectrode("ant")->getMERData(_MERClient->getCurrentDepth());
                if(data->getRecordedSeconds() >= 5 && data->getRecordedSeconds()-lastRecord > 0){
                    data->executeFFTWelch(5,true);
                    perceptronInputs = data->getSpectralPowerNormalizedAndWindowed();
                    perceptronInputs.push_back(_MERClient->getCurrentDepth());
                    _perceptron->setInputs(perceptronInputs);
                    _perceptron->classify();
                }

                data = bundle->getElectrode("cen")->getMERData(_MERClient->getCurrentDepth());
                if(data->getRecordedSeconds() >= 5 && data->getRecordedSeconds()-lastRecord > 0){
                    data->executeFFTWelch(5,true);
                    perceptronInputs = data->getSpectralPowerNormalizedAndWindowed();
                    perceptronInputs.push_back(_MERClient->getCurrentDepth());
                    _perceptron->setInputs(perceptronInputs);
                    _perceptron->classify();
                }
                if(data->getRecordedSeconds()-lastRecord > 0){
                    lastRecord = data->getRecordedSeconds();
                    if( DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset()) != nullptr)
                        DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset())->incrementStatus();
                }

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

void MERTool::on_classifyButton_clicked()
{
    std::shared_ptr<MERBundle> bundle =  MERBundleManager::getInstance().getMERBundle(MERBundleManager::getInstance().getActiveBundleName());

    std::vector<double> d;
    std::shared_ptr<MERElectrode> electrode;

    for(int j = 0; j < 3;++j){
        switch(j){
        case 0 :
            electrode = bundle->getElectrode("lat"); break;
        case 1 :
            electrode = bundle->getElectrode("cen"); break;
        case 2 :
            electrode = bundle->getElectrode("ant"); break;
        default : electrode = nullptr;
        }
        if(electrode == nullptr) continue;

        for(int i = -10; i <= 5; ++i){
            std::shared_ptr<BrainVisIO::MERData::MERData> data = electrode->getMERData(i);
            if(data == nullptr) continue;

            d = electrode->getMERData(i)->getSpectralPowerNormalizedAndWindowed();
            d.push_back(i);

            _perceptron->setInputs(d);
            if(_perceptron->weights().size() != d.size()) _perceptron->setBaseWeights(d.size());

            data->setIsSTNclassified(_perceptron->classify());
        }
    }

    updateData(MERBundleManager::getInstance().getActiveBundleName());
}

void MERTool::on_learnelectrodeButton_clicked()
{
    std::shared_ptr<MERBundle> bundle =  MERBundleManager::getInstance().getMERBundle(MERBundleManager::getInstance().getActiveBundleName());
    learnElectrodeBundle(bundle);

    for(const double d : _perceptron->weights()){
        std::cout << d << std::endl;
    }
    _perceptron->saveWeights();
}

void MERTool::on_learnAllelectrodeButton_clicked()
{
   std::vector<std::string> bundles =  MERBundleManager::getInstance().getRegisteredBundles();
   std::shared_ptr<MERBundle> bundle;

   bool madeChangesCounter = 0;

   do{
       madeChangesCounter = 0;
       for(int i = 0; i < bundles.size();++i){
            bundle = MERBundleManager::getInstance().getMERBundle(bundles[i]);
            if(!learnElectrodeBundle(bundle)){
                madeChangesCounter++;
            }
       }
   }while(madeChangesCounter != 0);

   for(const double d : _perceptron->weights()){
       std::cout << d << std::endl;
   }
   _perceptron->saveWeights();
}

void MERTool::on_classificationBox_clicked()
{
    if(ui->classificationBox->isChecked()){
        ui->classificationFrame->show();
    }else{
        ui->classificationFrame->hide();
    }
}

bool MERTool::learnElectrodeBundle(std::shared_ptr<MERBundle> bundle){
    std::vector<double> d;
    std::shared_ptr<MERElectrode> electrode;

    bool allcorrect = true;
    int notCorrectCounter = 0;
    do{
        notCorrectCounter = 0;
        for(int j = 0; j < 3;++j){
            switch(j){
            case 0 :
            electrode = bundle->getElectrode("lat"); break;
            case 1 :
            electrode = bundle->getElectrode("cen"); break;
            case 2 :
            electrode = bundle->getElectrode("ant"); break;
            default : electrode = nullptr;
            }
            if(electrode == nullptr) continue;

            for(int i = -10; i <= 5; ++i){
                std::shared_ptr<BrainVisIO::MERData::MERData> data = electrode->getMERData(i);
                if(data == nullptr) continue;

                d = electrode->getMERData(i)->getSpectralPowerNormalizedAndWindowed();
                d.push_back(i);

                _perceptron->setInputs(d);
                if(_perceptron->weights().size() != d.size()) _perceptron->setBaseWeights(d.size());

                if(!_perceptron->train(data->getIsSTNclassified())){
                    notCorrectCounter++;
                    allcorrect = false;
                }
            }
        }
    }while(notCorrectCounter != 0);

    return allcorrect;
}

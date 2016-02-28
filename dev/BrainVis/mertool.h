#ifndef MERTOOL_H
#define MERTOOL_H

#include <QDockWidget>

#include <merwidgets/meroptions.h>

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>
#include <BrainVisIO/MERNetworking/MERClient.h>

#include "merwidgets/merelectrodeentry.h"
#include "merwidgets/mertoolenums.h"

#include "Utils/Perceptron.h"

#include <vector>
#include <memory>
#include <map>
#include <thread>

namespace Ui {
class MERTool;
}

class MERTool : public QDockWidget
{
    Q_OBJECT

public:
    explicit MERTool(QWidget *parent = 0);
    ~MERTool();

    void closeEvent(QCloseEvent *event);

    void fftCalcThreadRun();
    void disableSelection();

    int timerId;
protected:

    void timerEvent(QTimerEvent *event);

private slots:
    void on_checkBox_clicked();

    void on_loadButton_clicked();

    void on_BundleSelection_activated(const QString &arg1);


    void on_signalRadio_toggled(bool checked);

    void on_fftRadio_toggled(bool checked);

    void on_classifierRadio_toggled(bool checked);

    void on_connectButton_clicked();

    void on_recordButton_clicked();

    void on_nextButton_clicked();

    void on_saveButton_clicked();

    void on_optionsButton_clicked();

    void on_disconnectButton_clicked();

    void on_classifyButton_clicked();

    void on_learnelectrodeButton_clicked();

    void on_learnAllelectrodeButton_clicked();

    void on_classificationBox_clicked();

private:
    bool learnElectrodeBundle(std::shared_ptr<BrainVisIO::MERData::MERBundle> bundle);
    void updateData(const std::string& bundlename);
    void updateSettings(const std::shared_ptr<BrainVisIO::MERData::MERBundle> bunlde);
    void loadDataBase();

    Ui::MERTool *ui;

    std::map<std::string, std::shared_ptr<merelectrodeentry>> _electrodeEntries;
    MERDisplayMode                                            _displayMode;

    std::unique_ptr<MERClient>                                _MERClient;

    bool                                                      _fftThreadStop;
    std::unique_ptr<std::thread>                              _fftCalcThread;

    std::shared_ptr<MERRecordSettings>                        _currentRecordingSettings;

    std::unique_ptr<Perceptron>                               _perceptron;
};

#endif // MERTOOL_H

#include <QFileDialog>

#include "planingwidget.h"
#include "ui_planingwidget.h"

#include <string>

#include <BrainVisIO/DataHandle.h>
#include <mainwindow.h>
#include <renderwidget.h>

#include <histogrammwidget.h>
#include <framewidget.h>


PlaningWidget::PlaningWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PlaningWidget)
{
    ui->setupUi(this);

    this->setFloating(true);
}

PlaningWidget::~PlaningWidget()
{
    delete ui;
}

void PlaningWidget::on_ctButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString fileName = dialog.getExistingDirectory();

    ui->CTPath->setText(fileName);

}

void PlaningWidget::on_mrButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString fileName = dialog.getExistingDirectory();

    ui->MRPath->setText(fileName);
}

void PlaningWidget::on_Start_clicked()
{
    std::shared_ptr<DataHandle> _dataHandle = std::make_shared<DataHandle>();

    MainWindow* w = (MainWindow*)this->parent();

    _dataHandle->setCTPath(ui->CTPath->text().toLocal8Bit().constData());
    _dataHandle->setMRPath(ui->MRPath->text().toLocal8Bit().constData());

    //AC-PC
    _dataHandle->setAC(Vec3f(ui->ACX->text().toFloat(),ui->ACY->text().toFloat(),ui->ACZ->text().toFloat()));
    _dataHandle->setPC(Vec3f(ui->PCX->text().toFloat(),ui->PCY->text().toFloat(),ui->PCZ->text().toFloat()));
    _dataHandle->setMR(Vec3f(ui->MRX->text().toFloat(),ui->MRY->text().toFloat(),ui->MRZ->text().toFloat()));


    _dataHandle->setBFoundCTFrame(false);

    std::string ct = _dataHandle->getCTPath();
    std::string mr = _dataHandle->getMRPath();

    if(_dataHandle->getCTPath().size() > 1){
        _dataHandle->loadCTData(ct);
    }

    if(_dataHandle->getMRPath().size() > 1){
        _dataHandle->loadMRData(mr);
    }

    w->setDataHandle(_dataHandle);

    w->createNewRenderWidger();

    /*HistogrammWidget* hw = new HistogrammWidget(w,_dataHandle);
    hw->createHistogramms(_dataHandle->getCTHistogramm(),_dataHandle->getMRHistogramm());

    FrameWidget* fw = new FrameWidget(w,_dataHandle);*/

    this->close();
}

void PlaningWidget::on_LoadSettings_clicked()
{

}




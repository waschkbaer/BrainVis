#include <QFileDialog>

#include "planingwidget.h"
#include "ui_planingwidget.h"

#include <string>

#include <BrainVisIO/DataHandle.h>
#include <mainwindow.h>
#include <renderwidget.h>


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

    //target
    Vec3f lEntry = Vec3f(ui->eLeftX->text().toFloat(),ui->eLeftY->text().toFloat(),ui->eLeftZ->text().toFloat());
    Vec3f lTarget = Vec3f(ui->tLeftX->text().toFloat(),ui->tLeftY->text().toFloat(),ui->tLeftZ->text().toFloat());
    Vec3f rEntry = Vec3f(ui->eRightX->text().toFloat(),ui->eRightY->text().toFloat(),ui->eRightZ->text().toFloat());
    Vec3f rTarget = Vec3f(ui->tRightX->text().toFloat(),ui->tRightY->text().toFloat(),ui->tRightZ->text().toFloat());

    _dataHandle->setLeftSTN(Trajectory(lEntry,lTarget));
    _dataHandle->setRightSTN(Trajectory(rEntry,rTarget));

    _dataHandle->setBFoundCTFrame(false);

    std::string ct = _dataHandle->getCTPath();
    std::string mr = _dataHandle->getMRPath();

    if(_dataHandle->getCTPath().size() > 1){
        _dataHandle->loadCTData(ct);
    }

    if(_dataHandle->getMRPath().size() > 1){
        _dataHandle->loadMRData(mr);
    }



    //load mer data
    std::vector<std::string> electrodes;
    if(ui->trLat->isChecked()){
        electrodes.push_back("LLat");
        electrodes.push_back("RLat");
    }
    if(ui->trAnt->isChecked()){
        electrodes.push_back("LAnt");
        electrodes.push_back("RAnt");
    }
    if(ui->trCen->isChecked()){
        electrodes.push_back("LCen");
        electrodes.push_back("RCen");
    }
    std::string merPath =ui->MERPath_2->text().toLocal8Bit().constData();
    std::string suffix = "\\";
    if(merPath.compare(merPath.size() - suffix.size(), suffix.size(), suffix) != 0){
        std::cout << "does not end with \\"<<std::endl;
        merPath+= "\\";
    }
    if(!ui->mernetworkbox->isChecked()){
        _dataHandle->loadMERFiles(merPath,electrodes);
    }else{
        std::string merHost =ui->merip->text().toLocal8Bit().constData();
        int port = ui->merport->text().toInt();

        _dataHandle->loadMERNetwork(electrodes);
    }


    w->setDataHandle(_dataHandle);

    w->createNewRenderWidger();


    this->close();
}

void PlaningWidget::on_LoadSettings_clicked()
{

}



void PlaningWidget::on_merButton_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString fileName = dialog.getExistingDirectory();

    ui->MERPath_2->setText(fileName);
}

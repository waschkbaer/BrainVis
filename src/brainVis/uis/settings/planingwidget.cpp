#include <QFileDialog>

#include "planingwidget.h"
#include "ui_planingwidget.h"

#include <string>

#include "uis/mainwindow.h"
#include "uis/widgets/renderwidget.h"

#include "uis/widgets/histogrammwidget.h"
#include "uis/tools/framewidget.h"

#include <ActivityManager.h>
#include <io/DataHandleManager.h>

using namespace BrainVis;

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

    uint16_t handle = DataHandleManager::getInstance().createNewDataHandle();
    ActivityManager::getInstance().setActiveDataset(handle);

    std::shared_ptr<DataHandle> dataHandle = DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset());

    MainWindow* w = (MainWindow*)this->parent();

    dataHandle->setCTPath(ui->CTPath->text().toLocal8Bit().constData());
    dataHandle->setMRPath(ui->MRPath->text().toLocal8Bit().constData());

    //AC-PC
    dataHandle->setAC(Vec3f(ui->ACX->text().toFloat(),ui->ACY->text().toFloat(),ui->ACZ->text().toFloat()));
    dataHandle->setPC(Vec3f(ui->PCX->text().toFloat(),ui->PCY->text().toFloat(),ui->PCZ->text().toFloat()));
    dataHandle->setMR(Vec3f(ui->MRX->text().toFloat(),ui->MRY->text().toFloat(),ui->MRZ->text().toFloat()));


    dataHandle->setBFoundCTFrame(false);

    std::string ct = dataHandle->getCTPath();
    std::string mr = dataHandle->getMRPath();

    try{
        if(dataHandle->getCTPath().size() > 1){
            dataHandle->loadCTData(ct);
        }
        if(dataHandle->getMRPath().size() > 1){
            dataHandle->loadMRData(mr);
        }
    }catch(std::exception &e){
        DataHandleManager::getInstance().deleteDataHandle(handle);
        this->close();
        return;
    }
    w->setDataHandle(dataHandle);

    w->createNewRenderWidget(RenderMode::YAxis, dockMode::center);
    w->createNewRenderWidget(RenderMode::XAxis, dockMode::bottom);
    w->createNewRenderWidget(RenderMode::ZAxis, dockMode::top);
    w->createNewRenderWidget(RenderMode::ThreeDMode, dockMode::left);
    this->close();
}

void PlaningWidget::on_LoadSettings_clicked()
{

}




#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "renderwidget.h"
#include <iostream>
#include <QDesktopWidget>
#include <QFileDialog>

#include "ModiSingleton.h"

#include "planingwidget.h"

static std::string path = "C:\\Users\\andre\\Dropbox\\MasterThesis\\Data\\MER Imagedata\\MRKopf_1118\\t1_vibe__3659";
//std::string path = "/Users/waschbaer/Dropbox/masterthesis/Data/MER\ Imagedata/MRKopf_1118/t1_vibe__3659";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _reloadData(true),
    _renderIDCounter(0)
{
    ui->setupUi(this);
    showMaximized();
}

MainWindow::~MainWindow()
{
    _data->waitForNetworkThread();
    delete ui;
}

void MainWindow::on_actionAdd_RenderWidget_triggered()
{
    if(_data != nullptr){
     m_vActiveWidgets.push_back(new RenderWidget(_data,this,this->getNextRenderIDCounter()));
     m_vActiveWidgets[m_vActiveWidgets.size()-1]->setFloating (true);
     }
}

void MainWindow::on_actionSelect_Folder_triggered()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    QString fileName = dialog.getExistingDirectory();
    path = fileName.toLocal8Bit().constData();
}


void MainWindow::on_actionNew_triggered()
{
    PlaningWidget* p = new PlaningWidget(this);
    p->setFloating(true);
    p->show();
}

void MainWindow::on_actionMove_triggered()
{
    ModiSingleton::getInstance().setActiveMode(Mode::CameraMovement);
}

void MainWindow::on_actionRotate_triggered()
{
    ModiSingleton::getInstance().setActiveMode(Mode::CameraRotation);
}

void MainWindow::on_actionWindowing_triggered()
{
    ModiSingleton::getInstance().setActiveMode(Mode::TFEditor);
}

void MainWindow::on_actionPicking_triggered()
{
    ModiSingleton::getInstance().setActiveMode(Mode::VolumePicking);
}
int MainWindow::getNextRenderIDCounter()
{
    _renderIDCounter++;
    return _renderIDCounter;
}


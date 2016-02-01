
#include <QDesktopWidget>
#include <QFileDialog>

#include "ctregistrationwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "renderwidget.h"
#include "framewidget.h"
#include "histogrammwidget.h"
#include "mertool.h"

#include "ModiSingleton.h"
#include "planingwidget.h"
#include <iostream>

#include <BrainVisIO/DataHandleManager.h>
#include <renderer/DICOMRenderer/DICOMRenderManager.h>
#include "ActivityManager.h"
#include <renderer/DICOMRenderer/DICOMRendererEnums.h>

using namespace BrainVis;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _reloadData(true),
    _histogramm(nullptr),
    _frame(nullptr),
    _registrationWidget(nullptr),
    _renderIDCounter(0)
{
    ui->setupUi(this);
    showMaximized();
    ui->centralWidget->setStyleSheet("background-color: #8D9294;");

    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(true);
}

MainWindow::~MainWindow()
{
    if(_data != nullptr && _data->getUsesNetworkMER()){
        _data->waitForNetworkThread();
    }
    delete ui;
}

void MainWindow::createNewRenderWidger(RenderMode mode,Core::Math::Vec2ui position){
    if(ActivityManager::getInstance().getActiveDataset() != -1){
        int renderHandle = DicomRenderManager::getInstance().addRenderer();
        ActivityManager::getInstance().setActiveRenderer(renderHandle);

        m_vActiveRenderer.push_back(std::make_shared<RenderWidget>(
                                        DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset()),
                                        this,
                                        renderHandle,
                                        mode,
                                        position)
                                   );


    }
}

int MainWindow::getNextRenderIDCounter()
{
    _renderIDCounter++;
    return _renderIDCounter;
}
void MainWindow::on_actionAdd_RenderWidget_triggered()
{
    createNewRenderWidger();
}

std::shared_ptr<RenderWidget> MainWindow::getWorkingRenderer(){
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        if(w != nullptr && w->isValid()){
            return w;
        }
    }
    return nullptr;
}

void MainWindow::removeRenderer(int id){
    for(int i = 0; i < m_vActiveRenderer.size();++i){
        if(m_vActiveRenderer[i] != nullptr && m_vActiveRenderer[i]->renderID() == id){
            m_vActiveRenderer[i]->Cleanup();
            m_vActiveRenderer[i] = nullptr;
            m_vActiveRenderer.erase(m_vActiveRenderer.begin()+i);
            i =  m_vActiveRenderer.size()+1;
            break;
        }
    }
}

void MainWindow::closeRegistrationWidget(){
    _registrationWidget = nullptr;
}

void MainWindow::closeFrameWidget(){
    _frame = nullptr;
}
void MainWindow::closeHistogrammWidget(){
    _histogramm = nullptr;
}

void MainWindow::closeMERTool(){
    _MERTool = nullptr;
}

void MainWindow::on_actionNew_triggered()
{
    PlaningWidget* p = new PlaningWidget(this);
    p->setFloating(true);
    p->show();
}

void MainWindow::on_actionMove_triggered()
{
    ModiSingleton::getInstance().setActiveModeLeftClick(Mode::CameraMovement);
    ui->actionRotate->setChecked(false);
    ui->actionMove->setChecked(true);
    DicomRenderManager::getInstance().setTrackMode(false);
}

void MainWindow::on_actionRotate_triggered()
{
    ModiSingleton::getInstance().setActiveModeLeftClick(Mode::CameraRotation);
    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(true);
    DicomRenderManager::getInstance().setTrackMode(true);
}

void MainWindow::on_actionWindowing_triggered()
{
    ModiSingleton::getInstance().setActiveModeRightClick(Mode::TFEditor);
}

void MainWindow::on_actionPicking_triggered()
{
    ModiSingleton::getInstance().setActiveModeRightClick(Mode::VolumePicking);
}

void MainWindow::disableControllBoxes(){
}

void MainWindow::on_actionCubic_Cut_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::CubicCut);
    }
}

void MainWindow::on_actionClip_Plane_automatic_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneAuto);
    }
}

void MainWindow::on_actionClip_Plane_Y_Axis_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneY);
    }
}

void MainWindow::on_actionClip_Plane_X_Axis_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneX);
    }
}

void MainWindow::on_actionClip_Plane_Z_Axis_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneZ);
    }
}

void MainWindow::on_actionClip_Plane_Y_Axis_2_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneYn);
    }
}

void MainWindow::on_actionClip_Plane_X_Axis_2_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneXn);
    }
}

void MainWindow::on_actionClip_Plane_Z_Axis_2_triggered()
{
    for(std::shared_ptr<RenderWidget> w : m_vActiveRenderer){
        w->setClipMode(DICOMClipMode::PlaneZn);
    }
}

void MainWindow::on_actionRegistration_Widget_triggered()
{
    if(_data != nullptr && _registrationWidget == nullptr){
        _registrationWidget = std::make_shared<CtRegistrationWidget>(this,_data);
    }
}

void MainWindow::on_actionNewRenderer_triggered()
{
    createNewRenderWidger();
}

void MainWindow::on_actionRegistration_triggered()
{
    if(_data != nullptr && _registrationWidget == nullptr){
        _registrationWidget = std::make_shared<CtRegistrationWidget>(this,_data);
    }
}

void MainWindow::on_actionDrive_Tool_triggered()
{
    if(_MERTool == nullptr){
        _MERTool = std::make_shared<MERTool>(this);
        this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea ,(QDockWidget*)(_MERTool.get()));
    }
}

void MainWindow::on_actionHistogramm_triggered()
{
    if(_data != nullptr && _histogramm == nullptr){
        _histogramm = std::make_shared<HistogrammWidget>(this,_data);
        _histogramm->createHistogramms(_data->getCTHistogramm(),_data->getMRHistogramm());
    }
}

void MainWindow::on_actionFrame_Detection_triggered()
{
    if(_data != nullptr && _frame == nullptr){
        _frame = std::make_shared<FrameWidget>(this,_data);
    }
}

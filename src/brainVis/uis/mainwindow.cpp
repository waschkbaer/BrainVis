
#include <QDesktopWidget>
#include <QFileDialog>

#include "uis/tools/ctregistrationwidget.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "uis/widgets/renderwidget.h"
#include "uis/tools/framewidget.h"
#include "uis/widgets/histogrammwidget.h"
#include "uis/widgets/mertool.h"

#include "ModiSingleton.h"
#include "uis/settings/planingwidget.h"
#include <iostream>

#include <io/DataHandleManager.h>
#include <renderer/DICOMRenderManager.h>
#include "ActivityManager.h"
#include <renderer/DICOMRendererEnums.h>

#include "uis/settings/rendersettings.h"
#include "uis/tools/muiconnector.h"
#include "muihandler/muihandler.h"

using namespace BrainVis;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _reloadData(true),
    _histogramm(nullptr),
    _frame(nullptr),
    _registrationWidget(nullptr),
    _muiConnector(nullptr),
    _renderIDCounter(0)
{
    ui->setupUi(this);
    ui->centralWidget->setStyleSheet("background-color: #8D9294;");

    ui->actionMove->setChecked(false);
    ui->actionRotate->setChecked(true);

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

MainWindow::~MainWindow()
{
    MuiHandler::getInstance().stop();
    if(_data != nullptr && _data->getUsesNetworkMER()){
        _data->waitForNetworkThread();
    }
    delete ui;
}

void MainWindow::createNewRenderWidget(RenderMode mode,dockMode dockmode){
    if(ActivityManager::getInstance().getActiveDataset() != -1){
        int renderHandle = DicomRenderManager::getInstance().addRenderer();
        ActivityManager::getInstance().setActiveRenderer(renderHandle);

        m_vActiveRenderer.push_back(std::make_shared<RenderWidget>(
                                        DataHandleManager::getInstance().getDataHandle(ActivityManager::getInstance().getActiveDataset()),
                                        this,
                                        renderHandle,
                                        mode,
                                        Vec2ui(0,0))
                                   );



        switch(dockmode){
            case dockMode::center: this->setCentralWidget((QWidget*)m_vActiveRenderer[m_vActiveRenderer.size()-1].get());break;
            case dockMode::left: this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea ,(QDockWidget*)m_vActiveRenderer[m_vActiveRenderer.size()-1].get());break;
            case dockMode::top: this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea ,(QDockWidget*)m_vActiveRenderer[m_vActiveRenderer.size()-1].get());break;
            case dockMode::bottom: this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea ,(QDockWidget*)m_vActiveRenderer[m_vActiveRenderer.size()-1].get());break;
            case dockMode::right:
            case dockMode::noDock: m_vActiveRenderer[m_vActiveRenderer.size()-1]->setFloating(true); break;
        }

    }
}

int MainWindow::getNextRenderIDCounter()
{
    _renderIDCounter++;
    return _renderIDCounter;
}
void MainWindow::on_actionAdd_RenderWidget_triggered()
{
    createNewRenderWidget();
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
    createNewRenderWidget();
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

void MainWindow::on_actionClipping_X_triggered()
{
    on_actionClip_Plane_X_Axis_triggered();
}

void MainWindow::on_actionClipping_X_minus_triggered()
{
    on_actionClip_Plane_X_Axis_2_triggered();
}

void MainWindow::on_actionClipping_Y_triggered()
{
    on_actionClip_Plane_Y_Axis_triggered();
}

void MainWindow::on_actionClipping_Y_minus_triggered()
{
    on_actionClip_Plane_Y_Axis_2_triggered();
}

void MainWindow::on_actionClipping_Z_triggered()
{
    on_actionClip_Plane_Z_Axis_triggered();
}

void MainWindow::on_actionClipping_Z_minus_triggered()
{
    on_actionClip_Plane_Z_Axis_2_triggered();
}

void MainWindow::on_actionCubicCut_triggered()
{
    on_actionCubic_Cut_triggered();
}

void MainWindow::on_actionRenderSettings_triggered()
{
    RenderSettings* r = new RenderSettings(this);
    r->show();
}

void MainWindow::on_actionSolidBlend_triggered()
{
    if(DicomRenderManager::getInstance().getBlendoption() == 0){
        DicomRenderManager::getInstance().setBlendoption(1);
    }else{
        DicomRenderManager::getInstance().setBlendoption(0);
    }
}

void MainWindow::on_actionMorphableUI_triggered()
{
   if(_muiConnector == nullptr){
       _muiConnector = std::make_shared<MuiConnector>(this);
   }
}

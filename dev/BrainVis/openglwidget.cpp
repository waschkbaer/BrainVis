#include <QString>
#include <QDockWidget>
#include "openglwidget.h"
#include <ModiSingleton.h>

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>

#include "Utils/FontImagePainter.h"
#include <BrainVisIO/DataHandleManager.h>
#include <renderer/DICOMRenderer/DICOMRenderManager.h>
#include "ActivityManager.h"
#include <renderer/Context/GLMutex.h>

using namespace BrainVis;
using namespace Tuvok::Renderer;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      _leftMouseDown(false),
      _rightMouseDown(false),
      _windowSize(0,0),
      _scrollMode(false),
      _rendererID(-1),
      _renderer(nullptr)
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
    _timer->start(16);
    installEventFilter(this);
}

OpenGLWidget::~OpenGLWidget()
{
}

void OpenGLWidget::Cleanup(){
    GLMutex::getInstance().lockContext();
    makeCurrent();
    _renderer->Cleanup();
    doneCurrent();
    GLMutex::getInstance().unlockContext();
}


void OpenGLWidget::initializeGL()
{
    GLMutex::getInstance().lockContext();
    makeCurrent();
    initializeOpenGLFunctions();


    const char* versionString = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    std::string t(versionString, 0, 17);
    std::cout << "OpenGL Version : "<< t << std::endl;

    GLenum err = glewInit();

    //If GLEW hasn't initialized
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    GLMutex::getInstance().unlockContext();
}

int i=0;
void OpenGLWidget::paintGL(){

    GLMutex::getInstance().lockContext();
    makeCurrent();

    if(_renderer != nullptr){
        if(_windowSize.x != width() ||_windowSize.y != height() ){
            _windowSize.x = width();
            _windowSize.y = height();
            _renderer->SetWindowSize(width(),height());
        }

        renderFont();
        _renderer->Paint();

    }else if(_rendererID != -1){
        _renderer = DicomRenderManager::getInstance().getRenderer(_rendererID);
        _renderer->SetDataHandle(_data);
        _renderer->Initialize();
    }
    //doneCurrent();
    GLMutex::getInstance().unlockContext();
}

Vec2i oldPos(-1,-1);
void OpenGLWidget::mousePressEvent(QMouseEvent * event ){
    if(event->button() == Qt::LeftButton){
        _leftMouseDown=true;
        mouseMoveEvent(event);
    }else if(event->button() == Qt::RightButton){
        _rightMouseDown=true;
    }
}
void OpenGLWidget::mouseReleaseEvent (QMouseEvent * event ){
    if(event->button() == Qt::LeftButton){
        _leftMouseDown=false;
        oldPos.x = -1;
        oldPos.y = -1;
    }else if(event->button() == Qt::RightButton){
        _rightMouseDown=false;
        mouseMoveEvent(event);
        oldPos.x = -1;
        oldPos.y = -1;
    }

}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event){
    ActivityManager::getInstance().setActiveRenderer(_rendererID);
    GLMutex::getInstance().lockContext();
    if(_leftMouseDown){

        //camera movement active
        if(ModiSingleton::getInstance().getActiveModeLeftClick() == Mode::CameraMovement){
            if(oldPos.x == -1 && oldPos.y == -1){
                oldPos.x = event->pos().x();
                oldPos.y = event->pos().y();
            }

            Vec2i delta(event->pos().x()-oldPos.x ,event->pos().y()-oldPos.y);
            _renderer->moveCamera(Vec3f(-delta.x*0.01f,delta.y*0.01f,0));

        //camera rotation active
        }else if(ModiSingleton::getInstance().getActiveModeLeftClick() == Mode::CameraRotation){
            if(oldPos.x == -1 && oldPos.y == -1){
                oldPos.x = event->pos().x();
                oldPos.y = event->pos().y();
            }

            Vec2i delta(event->pos().x()-oldPos.x ,event->pos().y()-oldPos.y);
            _renderer->rotateCamera(Vec3f(delta.y*0.5f,delta.x*0.5f,0));

        }
        oldPos.x = event->pos().x();
        oldPos.y = event->pos().y();
    }
    if(_rightMouseDown){
        //pickPosition(Core::Math::Vec2ui(event->pos().x(),event->pos().y()));

        //right click tf editor!
        if(ModiSingleton::getInstance().getActiveModeRightClick() == Mode::TFEditor){

            updateTF((float)event->pos().x(),(float)event->pos().y(),(float)width(),(float)height());

        //volume picking active
        }else if(ModiSingleton::getInstance().getActiveModeRightClick() == Mode::VolumePicking){
            std::cout << "renderer: " << rendererID() <<std::endl;
            _renderer->PickPixel(Vec2ui(event->pos().x(),event->pos().y()));
        }
        oldPos.x = event->pos().x();
        oldPos.y = event->pos().y();
    }
    GLMutex::getInstance().unlockContext();
}

void OpenGLWidget::switchScrollMode(){
    _scrollMode = !_scrollMode;
}

void OpenGLWidget::wheelEvent(QWheelEvent *event){
    if(_scrollMode){
        changeSlide(event->delta()/120);
    }else{
        zoom(event->delta()/120);
    }
}

void OpenGLWidget::updateTF(float xPos, float yPos, float widgetSizeX, float widgetSizeY){
    DicomRenderManager::getInstance().setSmoothStep(xPos/widgetSizeX, yPos/widgetSizeY);
}

void OpenGLWidget::pickPosition(Core::Math::Vec2ui position){
    //_renderer->pickStuff xyz do magic BLABLABLA
}
int OpenGLWidget::rendererID() const
{
    return _rendererID;
}

void OpenGLWidget::setRendererID(int rendererID)
{
    _rendererID = rendererID;
}

void OpenGLWidget::renderFont(){
    if(_fontImage == nullptr){
        _fontImage = std::unique_ptr<FontImagePainter>(new FontImagePainter(width(),height()));
    }
    if( _fontImage->getImageSize().x != width() ||
        _fontImage->getImageSize().y != height()){
        _fontImage->resizeImage(width(),height());
    }else{
        _fontImage->clearImage();

        //draw all stuff needed
        _fontImage->setFontColor(0,128,255);
        //string : left electrode:[ depth -5, {LAnt: ?}{LCen: ?}{LLat: ?} ]
        std::string leftElec = "electrode info here";
        //_fontImage->drawText(2,14,leftElec);

        _fontImage->setFontColor(0,128,255);
        std::string rightElec = "electrode info here";
        //_fontImage->drawText(2,28,rightElec);

        _fontImage->setFontColor(255,255,0);
        std::string pickPosition = "targeted ("+
                                    std::to_string(_data->getSelectedCTSpacePosition().x)+
                                    ","+
                                    std::to_string(_data->getSelectedCTSpacePosition().y)+
                                    ","+
                                    std::to_string(_data->getSelectedCTSpacePosition().z)+
                                    ") MR Value: "+
                                    std::to_string(_data->getMriValue());

        _fontImage->drawText(2,height()-2,pickPosition);
        _fontImage->finishText();
        _renderer->setFontData((char*)_fontImage->getImageData());
    }
}

void OpenGLWidget::setClipMode(DICOMClipMode mode){
     _renderer->setClipMode(mode);
}

void OpenGLWidget::setRenderMode(RenderMode mode) {
    _renderer->SetRenderMode(mode);
}

void OpenGLWidget::setDataHandle(std::shared_ptr<DataHandle> data){
    _data = data;
}

void OpenGLWidget::changeSlide(int slidedelta){
    _renderer->ChangeSlide(slidedelta);
}

void OpenGLWidget::zoom(int zoomdelta){
    _renderer->ZoomTwoD(zoomdelta);
}

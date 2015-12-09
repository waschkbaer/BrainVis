#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QMouseEvent>
#include <QTimer>

#include <ModiSingleton.h>

#include "Utils/FontImagePainter.h"

#include <renderer/DICOMRenderer/DICOMRenderer.h>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <core/Math/Vectors.h>
#include <BrainVisIO/DataHandle.h>

#include <memory>
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    OpenGLWidget(QWidget *parent = 0);
        ~OpenGLWidget();

    void cleanup();
    void initializeGL();
    void paintGL();

    void update();

    void setRenderMode(DICOMRenderer::RenderMode mode) const {
        _renderer->SetRenderMode(mode);
    }

    void setDataHandle(std::shared_ptr<DataHandle> data){
        _renderer->SetDataHandle(data);
        _data = data;
    }

    void changeSlide(int slidedelta){
        _renderer->ChangeSlide(slidedelta);
    }

    void zoom(int zoomdelta){
        _renderer->ZoomTwoD(zoomdelta);
    }

    void mousePressEvent(QMouseEvent * event );
    void mouseReleaseEvent (QMouseEvent * event );
    void mouseMoveEvent(QMouseEvent *eventMove);
    void wheelEvent(QWheelEvent *event);

    void updateTF(float xPos, float yPos, float widgetSizeX, float widgetSizeY);

    void pickPosition(Core::Math::Vec2ui position);

    int rendererID() const;
    void setRendererID(int rendererID);

    void renderFont();

    void switchScrollMode();

    void setClipMode(DICOMClipMode mode){
         _renderer->setClipMode(mode);
    }

    void setDoGradientDescent(bool value){
        _renderer->setDoesGradientDescent(value);
    }

private:
    std::unique_ptr<DICOMRenderer>  _renderer;
    std::shared_ptr<DataHandle>     _data;
    QTimer*                         _timer;

    std::unique_ptr<FontImagePainter> _fontImage;

    Vec2ui                          _windowSize;

    bool                            _leftMouseDown;
    bool                            _rightMouseDown;

    bool                            _scrollMode;

    int                             _rendererID;
signals:

public slots:
};

#endif // OPENGLWIDGET_H

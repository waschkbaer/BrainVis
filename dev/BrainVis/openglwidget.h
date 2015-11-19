#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <renderer/DICOMRenderer/DICOMRenderer.h>

#include <QOpenGLWidget>

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QTimer>
#include <QPoint>

#include <core/Math/Vectors.h>
#include <BrainVisIO/DataHandle.h>

#include <memory>
#include "Utils/FontImagePainter.h"


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

    void mousePressEvent(QMouseEvent * event );
    void mouseReleaseEvent (QMouseEvent * event );
    void mouseMoveEvent(QMouseEvent *eventMove);
    void wheelEvent(QWheelEvent *event);

    void updateTF(float xPos, float yPos, float widgetSizeX, float widgetSizeY);

    void pickPosition(Core::Math::Vec2ui position);

    int rendererID() const;
    void setRendererID(int rendererID);

    void renderFont();

private:
    std::unique_ptr<DICOMRenderer>  _renderer;
    std::shared_ptr<DataHandle>     _data;
    QTimer*                         _timer;

    std::unique_ptr<FontImagePainter> _fontImage;

    Vec2ui                          _windowSize;

    bool                            _leftMouseDown;
    bool                            _rightMouseDown;

    int                             _rendererID;
signals:

public slots:
};

#endif // OPENGLWIDGET_H

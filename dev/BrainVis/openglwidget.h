#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QCursor> //will get error on linux if we do not include this WTF
#include <QMouseEvent>
#include <QTimer>

#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <memory>
#include <core/Math/Vectors.h>

#include <renderer/DICOMRenderer/DICOMRendererEnums.h>

class FontImagePainter;
class DICOMRenderer;
class DataHandle;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    OpenGLWidget(QWidget *parent = 0);
        ~OpenGLWidget();

    void cleanup();
    void initializeGL();
    void paintGL();

    void update();

    void setRenderMode(RenderMode mode);

    void setDataHandle(std::shared_ptr<DataHandle> data);

    void changeSlide(int slidedelta);

    void zoom(int zoomdelta);

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

    void setClipMode(DICOMClipMode mode);

    void setDoGradientDescent(bool value);

private:
    std::unique_ptr<DICOMRenderer>  _renderer;
    std::shared_ptr<DataHandle>     _data;
    QTimer*                         _timer;

    std::unique_ptr<FontImagePainter> _fontImage;

    Core::Math::Vec2ui                _windowSize;

    bool                            _leftMouseDown;
    bool                            _rightMouseDown;

    bool                            _scrollMode;

    int                             _rendererID;
signals:

public slots:
};

#endif // OPENGLWIDGET_H

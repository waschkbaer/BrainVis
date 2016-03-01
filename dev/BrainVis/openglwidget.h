#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QCursor> //will get error on linux if we do not include this WTF
#include <QMouseEvent>
#include <QTimer>
#include <renderer/DICOMRenderer/DICOMRenderer.h>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include <memory>
#include <core/Math/Vectors.h>

#include <renderer/DICOMRenderer/DICOMRendererEnums.h>

class FontImagePainter;
class DICOMRenderer;
class DataHandle;

/*!
 * \brief This is the actual OpenGL context widget used for the DICOMRenderer
 *
 * while the rendererwidget is a container for multiple widgets, like buttons or
 * the OpenGLWidget, the OpenGLWidget is the real rendering part of the renderer.
 * This class will create and initialize a new OpenGLContext and load the DICOMRenderer
 * used to visualize the Datasets
 */
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    OpenGLWidget(QWidget *parent = 0);
        ~OpenGLWidget();

    /*!
     * \brief Cleanup
     */
    void Cleanup();

    /*!
     * \brief initializeGL @overload
     *
     * This function loads the GLFunctions and glew. It is nessecary to load
     * the functions to get full access to an OpenGL 3.3+ Context
     */
    void initializeGL();

    /*!
     * \brief paintGL @overload
     *
     * This function is used to render our images. It will also take care of resizing
     * all our buffers inside the DICOMRenderer.
     */
    void paintGL();

    /*!
     * \brief sets the displaymode of the DICOMRenderer
     * \param mode can be one of the RenderMode
     */
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

private:
    std::shared_ptr<DICOMRenderer>      _renderer;
    std::shared_ptr<DataHandle>         _data;
    QTimer*                             _timer;

    std::unique_ptr<FontImagePainter>   _fontImage;

    Core::Math::Vec2ui                  _windowSize;

    bool                                _leftMouseDown;
    bool                                _rightMouseDown;

    bool                                _scrollMode;
    bool                                _initDone;

    int                                 _rendererID;
signals:

public slots:
};

#endif // OPENGLWIDGET_H

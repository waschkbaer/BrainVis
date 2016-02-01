#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QDockWidget>
#include <QCloseEvent>

#include <memory>

#include <renderer/DICOMRenderer/DICOMRendererEnums.h>
#include <core/Math/Vectors.h>

class DataHandle;

namespace Ui {
class RenderWidget;
}

class RenderWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(std::shared_ptr<DataHandle> data,
                          QWidget *parent = 0,
                          int renderID = 0,
                          RenderMode mode = RenderMode::ThreeDMode,
                          Core::Math::Vec2ui windowPosition = Core::Math::Vec2ui(0,80));
    ~RenderWidget();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    int renderID() const;

    void setClipMode(DICOMClipMode mode);

    void closeEvent(QCloseEvent *bar);

    void Cleanup();

    bool isValid() const;

private slots:
    void on_ThreeD_clicked();

    void on_ZAxis_clicked();

    void on_XAxis_clicked();

    void on_YAxis_clicked();

private:
    Ui::RenderWidget *ui;
    std::shared_ptr<DataHandle> _data;
    int _renderID;
    bool _isValid;
};

#endif // RENDERWIDGET_H

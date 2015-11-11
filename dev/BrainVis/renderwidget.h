#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QDockWidget>
#include <BrainVisIO/DataHandle.h>
#include <openglwidget.h>
#include <QWheelEvent>

#include "ModiSingleton.h"

namespace Ui {
class RenderWidget;
}

class RenderWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(std::shared_ptr<DataHandle> data, QWidget *parent = 0, int renderID = 0);
    ~RenderWidget();

        void keyPressEvent(QKeyEvent *event);

        int renderID() const;

private slots:
        void on_ThreeD_clicked();

    void on_ZAxis_clicked();

    void on_XAxis_clicked();

    void on_YAxis_clicked();



private:
    Ui::RenderWidget *ui;
    std::shared_ptr<DataHandle> _data;
    int _renderID;
};

#endif // RENDERWIDGET_H

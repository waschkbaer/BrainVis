#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QDockWidget>
#include <memory>

namespace Ui {
class FrameWidget;
}

class DataHandle;

class FrameWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit FrameWidget(QWidget *parent = 0, std::shared_ptr<DataHandle> data = nullptr);
    ~FrameWidget();

private slots:
    void on_leftActive_clicked();

    void on_rightActive_clicked();

    void on_mLR_sliderMoved(int position);

    void on_mFB_sliderMoved(int position);

    void on_mUD_sliderMoved(int position);

    void on_sUD_sliderMoved(int position);

    void on_sLR_sliderMoved(int position);

    void on_sFB_sliderMoved(int position);

    void on_pushButton_clicked();

private:
    Ui::FrameWidget *ui;
    std::shared_ptr<DataHandle> _data;

    void setSliders();
};

#endif // FRAMEWIDGET_H

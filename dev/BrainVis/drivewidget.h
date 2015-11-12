#ifndef DRIVEWIDGET_H
#define DRIVEWIDGET_H

#include <QDockWidget>
#include <QImage>
#include <QFrame>
#include <BrainVisIO/DataHandle.h>

#include <map>

namespace Ui {
class DriveWidget;
}

class DriveWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DriveWidget(QWidget *parent = 0, std::shared_ptr<DataHandle> data = nullptr);
    ~DriveWidget();

    void addTrajectory(std::string name, std::shared_ptr<iElectrode> electrode);
    void updateTrajectory(std::string name);

private:
    QFrame* createElectordeImageEntry(std::string name, int depth);

private slots:
    void on_verticalSlider_sliderMoved(int position);

private:
    Ui::DriveWidget *ui;
    std::shared_ptr<DataHandle> _data;
};

#endif // DRIVEWIDGET_H

#ifndef DRIVEWIDGET_H
#define DRIVEWIDGET_H

#include <QDockWidget>
#include <QImage>
#include <QFrame>
#include <BrainVisIO/DataHandle.h>
#include "DriveTool/ElectrodeBaseFrame.h"

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

    void updateWidget();

private:
    void addImageEntry(std::string name, int depth);

private slots:
    void on_verticalSlider_sliderMoved(int position);

    void on_addElectrode_clicked();

    void on_removeButton_clicked();

    void on_sginalButton_clicked();

    void on_probabilityButton_clicked();

    void on_fftButton_clicked();

private:
    Ui::DriveWidget *ui;
    std::shared_ptr<DataHandle> _data;

    std::map<std::string,ElectrodeBaseFrame*>      _electrodeFrames;
};

#endif // DRIVEWIDGET_H

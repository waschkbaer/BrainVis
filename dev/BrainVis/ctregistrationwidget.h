#ifndef CTREGISTRATIONWIDGET_H
#define CTREGISTRATIONWIDGET_H

#include <QDockWidget>
#include <memory>

namespace Ui {
class CtRegistrationWidget;
}

class DataHandle;
class DICOMRenderer;

class CtRegistrationWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CtRegistrationWidget(QWidget *parent = 0, std::shared_ptr<DataHandle> data = nullptr);
    ~CtRegistrationWidget();

    void closeEvent(QCloseEvent *bar);

private slots:
    void on_resetButton_clicked();

    void on_registerButton_clicked();

    void on_translationStepSize_sliderMoved(int position);

    void on_translationDegSize_sliderMoved(int position);

    void on_rotatioStepSize_sliderMoved(int position);

    void on_rotatioDegSize_sliderMoved(int position);

    void update();

private:
    Ui::CtRegistrationWidget *ui;
    std::shared_ptr<DataHandle> _data;

    float _translationStep;
    float _translationScaling;
    float _rotationStep;
    float _rotationScaling;

    std::shared_ptr<DICOMRenderer> _registrationRenderer;
};

#endif // CTREGISTRATIONWIDGET_H

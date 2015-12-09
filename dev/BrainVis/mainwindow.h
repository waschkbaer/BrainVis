#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>

#include <vector>
#include <memory>

namespace Ui {
class MainWindow;
}

class RenderWidget;
class CtRegistrationWidget;
class DataHandle;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setDataHandle(std::shared_ptr<DataHandle> d) {_data = d;}

    int getNextRenderIDCounter();

    void createNewRenderWidger();

    std::shared_ptr<RenderWidget> getWorkingRenderer();
    void removeRenderer(int id);
    void closeRegistrationWidget();

private slots:
    void on_actionAdd_RenderWidget_triggered();

    void on_actionNew_triggered();

    void on_actionMove_triggered();

    void on_actionRotate_triggered();

    void on_actionWindowing_triggered();

    void on_actionPicking_triggered();

    void on_actionCubic_Cut_triggered();

    void on_actionClip_Plane_automatic_triggered();

    void on_actionClip_Plane_Y_Axis_triggered();

    void on_actionClip_Plane_X_Axis_triggered();

    void on_actionClip_Plane_Z_Axis_triggered();

    void on_actionClip_Plane_Y_Axis_2_triggered();

    void on_actionClip_Plane_X_Axis_2_triggered();

    void on_actionClip_Plane_Z_Axis_2_triggered();

    void on_actionRegistration_Widget_triggered();

private:
    Ui::MainWindow *ui;

    std::vector<QDockWidget*>                           m_vActiveWidgets;
    std::vector<std::shared_ptr<RenderWidget>>          m_vActiveRenderer;
    std::shared_ptr<DataHandle>                         _data;
    bool                                                _reloadData;
    int                                                 _renderIDCounter;

    std::shared_ptr<CtRegistrationWidget>               _registrationWidget;
};

#endif // MAINWINDOW_H

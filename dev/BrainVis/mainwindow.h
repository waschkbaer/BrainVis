#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qtextstream.h>
#include <QMainWindow>

#include <vector>
#include <memory>
#include <BrainVisIO/DataHandle.h>

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setDataHandle(std::shared_ptr<DataHandle> d) {_data = d;}


    int getNextRenderIDCounter();

private slots:
    void on_actionAdd_RenderWidget_triggered();

    void on_actionSelect_Folder_triggered();

    void on_actionNew_triggered();

    void on_actionMove_triggered();

    void on_actionRotate_triggered();

    void on_actionWindowing_triggered();

    void on_actionPicking_triggered();

private:
    Ui::MainWindow *ui;

    std::vector<QDockWidget*>       m_vActiveWidgets;
    std::shared_ptr<DataHandle>     _data;
    bool                            _reloadData;
    int                             _renderIDCounter;
};

#endif // MAINWINDOW_H

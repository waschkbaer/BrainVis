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
class HistogrammWidget;
class FrameWidget;
class MERTool;

/*!
 * \brief The MainWindow acts as a central hub for all widgets
 *
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*!
     * \brief sets the shared_ptr of the DataHandle
     *
     * In the current version the application can hold a single
     * datahandle. The DataHandle consists of the patients CT/MR data
     * and the electrode information provided by the MER
     *
     * \param d shared_ptr to a new DataHandle instance.
     */
    void setDataHandle(std::shared_ptr<DataHandle> d) {_data = d;}

    /*!
     * \brief returns the next free ID for a new Renderer
     *
     * The returned ID can be used to access a certain renderer.
     *
     * \return int rendererID
     */
    int getNextRenderIDCounter();

    /*!
     * \brief creates a new widget for the registered DataHandle
     *
     * This call will create a new renderwidget with the current
     * active datahandle. By sharing the datahandle we reduce the
     * needed memory and achieve data synchronization between all
     * renderwidgets
     */
    void createNewRenderWidger();

    /*!
     * \brief returns a renderwidget to be used for some commands
     *
     * this function will return the first running renderwidget stored.
     * This function is mainly used for the CT and MRI registration which
     * needs a working renderer
     *
     * \return
     */
    std::shared_ptr<RenderWidget> getWorkingRenderer();

    /*!
     * \brief deletes a renderwidget with the provided id
     *
     * closes the renderer with the id and frees the memory
     *
     * \param id id of the renderer
     */
    void removeRenderer(int id);

    /*!
     * \brief frees the memory of the registration widget
     */
    void closeRegistrationWidget();
    void closeFrameWidget();
    void closeHistogrammWidget();

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

    void on_actionNewRenderer_triggered();

    void on_actionRegistration_triggered();

    void on_actionDrive_Tool_triggered();

    void on_actionHistogramm_triggered();

    void on_actionFrame_Detection_triggered();

private:
    Ui::MainWindow *ui;

    std::vector<QDockWidget*>                           m_vActiveWidgets;
    std::vector<std::shared_ptr<RenderWidget>>          m_vActiveRenderer;
    std::shared_ptr<DataHandle>                         _data;
    bool                                                _reloadData;
    int                                                 _renderIDCounter;

    std::shared_ptr<CtRegistrationWidget>               _registrationWidget;

    std::shared_ptr<HistogrammWidget>                   _histogramm;
    std::shared_ptr<FrameWidget>                        _frame;
    std::shared_ptr<MERTool>                            _MERTool;
};

#endif // MAINWINDOW_H

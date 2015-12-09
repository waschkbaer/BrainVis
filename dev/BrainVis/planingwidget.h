#ifndef PLANINGWIDGET_H
#define PLANINGWIDGET_H

#include <QDockWidget>


namespace Ui {
class PlaningWidget;
}

class PlaningWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit PlaningWidget(QWidget *parent = 0);
    ~PlaningWidget();

private slots:
    void on_ctButton_clicked();

    void on_Start_clicked();

    void on_LoadSettings_clicked();

    void on_mrButton_clicked();

    void on_merButton_clicked();

private:
    Ui::PlaningWidget *ui;
};

#endif // PLANINGWIDGET_H

#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QDockWidget>

namespace Ui {
class RenderSettings;
}

class RenderSettings : public QDockWidget
{
    Q_OBJECT

public:
    explicit RenderSettings(QWidget *parent = 0);
    ~RenderSettings();

private slots:
    void on_displayFrame_clicked();

    void on_displaydetectionbox_clicked();

    void on_displayboundingbox_clicked();

    void on_displayelectrodes_clicked();

    void on_performanceslider_valueChanged(int value);

    void on_blendingslider_valueChanged(int value);

private:
    Ui::RenderSettings *ui;
};

#endif // RENDERSETTINGS_H

#ifndef MERTOOL_H
#define MERTOOL_H

#include <QDockWidget>

namespace Ui {
class MERTool;
}

class MERTool : public QDockWidget
{
    Q_OBJECT

public:
    explicit MERTool(QWidget *parent = 0);
    ~MERTool();

    void update();

private slots:
    void on_checkBox_clicked();

private:
    Ui::MERTool *ui;
};

#endif // MERTOOL_H

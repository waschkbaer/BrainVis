#ifndef MUICONNECTOR_H
#define MUICONNECTOR_H

#include <QDockWidget>

namespace Ui {
class MuiConnector;
}

class MuiConnector : public QDockWidget
{
    Q_OBJECT

public:
    explicit MuiConnector(QWidget *parent = 0);
    ~MuiConnector();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MuiConnector *ui;
};

#endif // MUICONNECTOR_H

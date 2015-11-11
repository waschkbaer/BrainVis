#ifndef INFORMATIONWIDGET_H
#define INFORMATIONWIDGET_H

#include <QDockWidget>

namespace Ui {
class InformationWidget;
}

class InformationWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit InformationWidget(QWidget *parent = 0);
    ~InformationWidget();

private:
    Ui::InformationWidget *ui;
};

#endif // INFORMATIONWIDGET_H

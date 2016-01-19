#ifndef MERELECTRODEENTRY_H
#define MERELECTRODEENTRY_H

#include <QWidget>

namespace Ui {
class merelectrodeentry;
}

class merelectrodeentry : public QWidget
{
    Q_OBJECT

public:
    explicit merelectrodeentry(QWidget *parent = 0);
    ~merelectrodeentry();

private:
    Ui::merelectrodeentry *ui;
};

#endif // MERELECTRODEENTRY_H

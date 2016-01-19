#ifndef MERELECTRODEENTRY_H
#define MERELECTRODEENTRY_H

#include <QWidget>

#include <BrainVisIO/Data/MERElectrode.h>

namespace Ui {
class merelectrodeentry;
}

class merelectrodeentry : public QWidget
{
    Q_OBJECT

public:
    explicit merelectrodeentry(QWidget *parent = 0);
    ~merelectrodeentry();

    void createElectrodeEntries(std::shared_ptr<BrainVisIO::MERData::MERElectrode> electrode);

private:
    Ui::merelectrodeentry *ui;
};

#endif // MERELECTRODEENTRY_H

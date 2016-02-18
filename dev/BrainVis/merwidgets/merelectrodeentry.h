#ifndef MERELECTRODEENTRY_H
#define MERELECTRODEENTRY_H

#include <QWidget>

#include <BrainVisIO/Data/MERElectrode.h>

#include "mertoolenums.h"

namespace Ui {
class merelectrodeentry;
}

class merelectrodeentry : public QWidget
{
    Q_OBJECT

public:
    explicit merelectrodeentry(const std::string& electrodeName = "none", QWidget *parent = 0);
    ~merelectrodeentry();

    void createElectrodeEntries(std::shared_ptr<BrainVisIO::MERData::MERElectrode> electrode, MERDisplayMode mode = MERDisplayMode::fft);
    void clean();

private slots:
    void on_displayElectrode_clicked(bool checked);

private:
    Ui::merelectrodeentry *ui;

    std::vector<QWidget*>   _widgets;
    std::string             _electrodeName;
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> _electrode;
};

#endif // MERELECTRODEENTRY_H

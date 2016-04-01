#ifndef MERELECTRODEENTRY_H
#define MERELECTRODEENTRY_H

#include <QWidget>

#include <io/Data/MERElectrode.h>

#include "mertoolenums.h"
#include "merimageentry.h"


namespace Ui {
class merelectrodeentry;
}

class MERTool;

class merelectrodeentry : public QWidget
{
    Q_OBJECT

public:
    explicit merelectrodeentry(const std::string& electrodeName = "none", MERTool *parent = 0);
    ~merelectrodeentry();

    void createElectrodeEntries(std::shared_ptr<BrainVisIO::MERData::MERElectrode> electrode, MERDisplayMode mode = MERDisplayMode::fft);
    void clean();

    void disableAllImages();
    void disableSelection();

private slots:
    void on_displayElectrode_clicked(bool checked);

private:

    Ui::merelectrodeentry *ui;
    MERTool*            _parent;

    std::vector<MERimageentry*>             _widgets;
    std::string                             _electrodeName;
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> _electrode;
};

#endif // MERELECTRODEENTRY_H

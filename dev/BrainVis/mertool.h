#ifndef MERTOOL_H
#define MERTOOL_H

#include <QDockWidget>

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>

#include "merwidgets/merelectrodeentry.h"

#include <vector>
#include <memory>
#include <map>

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

    void on_loadButton_clicked();

private:
    Ui::MERTool *ui;

    std::map<std::string, std::shared_ptr<merelectrodeentry>> _electrodeEntries;
};

#endif // MERTOOL_H

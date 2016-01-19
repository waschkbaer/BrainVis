#include "merelectrodeentry.h"
#include "ui_merelectrodeentry.h"

merelectrodeentry::merelectrodeentry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::merelectrodeentry)
{
    ui->setupUi(this);
}

merelectrodeentry::~merelectrodeentry()
{
    delete ui;
}

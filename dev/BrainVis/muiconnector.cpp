#include "muiconnector.h"
#include "ui_muiconnector.h"

#include "muihandler/muihandler.h"

MuiConnector::MuiConnector(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MuiConnector)
{
    ui->setupUi(this);
    this->show();
    this->setFloating(true);
}

MuiConnector::~MuiConnector()
{
    delete ui;
}

void MuiConnector::on_pushButton_clicked()
{
    std::string hostname = ui->hostbox->text().toStdString();
    std::string port = ui->portbox->text().toStdString();
    MuiHandler::getInstance().connect(hostname,port);
}

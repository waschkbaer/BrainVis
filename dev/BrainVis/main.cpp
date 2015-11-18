#include "mainwindow.h"

#include <QApplication>

#include "drivewidget.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

   // DriveWidget* drive = new DriveWidget(&w);
   // drive->show();


    return a.exec();
}

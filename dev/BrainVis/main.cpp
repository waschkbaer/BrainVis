#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include "drivewidget.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);


    QSurfaceFormat format;
    format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();

   // DriveWidget* drive = new DriveWidget(&w);
   // drive->show();

    return a.exec();
}

/*! \mainpage BrainVis
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: check it out!
 *
 * etc...
 */

#include <QApplication>
#include <QSurfaceFormat>

#include "mainwindow.h"

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

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

#include <QFile>

#include <QApplication>
#include <QSurfaceFormat>

#include "uis/mainwindow.h"

#include <io/Data/MERData.h>
#include <io/Data/MERElectrode.h>
#include <io/Data/MERFileManager.h>
#include <io/Data/MERBundle.h>
#include <iostream>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile File("stylesheet.css");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    a.setStyleSheet(StyleSheet);


    MainWindow w;
    w.show();
    return a.exec();
}

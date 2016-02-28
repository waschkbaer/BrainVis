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

#include "mainwindow.h"

#include <BrainVisIO/Data/MERData.h>
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/SmallImage/SmallImage.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>
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

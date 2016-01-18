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

#include <BrainVisIO/Data/MERData.h>
#include <iostream>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    BrainVisIO::MERData::MERData data(-4,std::string("C:/Users/andre/Documents/BrainVis/build/bin/s_c_3_d_-4.txt"));
    std::vector<double> spectralPower = data.getSpectralPowerNormalized();

    return a.exec();
}

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
#include <BrainVisIO/Data/MERElectrode.h>
#include <BrainVisIO/SmallImage/SmallImage.h>
#include <BrainVisIO/Data/MERFileManager.h>
#include <BrainVisIO/Data/MERBundle.h>
#include <iostream>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    MainWindow w;
    w.show();


    //testing mer datastructure
    //BrainVisIO::MERData::MERData data(-4,std::string("C:/Users/andre/Documents/BrainVis/build/bin/s_c_3_d_-4.txt"));
    //std::vector<double> spectralPower = data.getSpectralPowerNormalized();

    //electrode test

    /*std::string path = "C:/Users/andre/Documents/BrainVis/build/bin/13_1_2016_m2_rechts";
    std::shared_ptr<BrainVisIO::MERData::MERBundle> right = BrainVisIO::MERData::MERFileManager::getInstance().openFolder(path);

    std::shared_ptr<BrainVisIO::MERData::MERElectrode> lat = right->getElectrode("lat");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> ant = right->getElectrode("ant");
    std::shared_ptr<BrainVisIO::MERData::MERElectrode> cen = right->getElectrode("cen");

    std::shared_ptr<BrainVisIO::MERData::MERData> d;
    IVDA::SmallImage image(656,100,3);
    std::vector<double> power;
    for(int i = -10;i <= 5; i++){
       d = lat->getMERData(i);
       power = d->getSpectralPowerNormalized();
       std::cout << power.size() << std::endl;

       //cleanImage
       for(int x = 0; x < image.Width();++x){
           for(int y = 0; y < image.Height();++y){
               image.SetPixel(x,y,0,0,0);
           }
       }

       for(int x = 0; x < image.Width();++x){
            int ymax = power[x]*(image.Height()-1);
            for(int y = 0; y < ymax;++y){
                image.SetPixel(x,y,255,255,255);
            }
       }
       image.SaveToBMPFile(std::string("test_lat_"+std::to_string(i)+".bmp"));
    }*/

    return a.exec();
}

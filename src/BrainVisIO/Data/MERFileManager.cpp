
#include "MERFileManager.h"
#include "MERBundle.h"
#include "MERData.h"
#include "MERElectrode.h"

#include <mocca/base/StringTools.h>

#include <iostream>
#include <fstream>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

using namespace BrainVisIO::MERData;

const std::shared_ptr<MERBundle> MERFileManager::openFolder(const std::string& folder){
        if(_bundles.find(folder) != _bundles.end()){
            return _bundles.find(folder)->second;
        }

        std::string fileString;

        std::vector<std::string> centerFiles;
        std::vector<std::string> lateralFiles;
        std::vector<std::string> anteriorFiles;

        for(int i = -10; i <= 5; ++i){
            for(int j = 0; j <= 5; ++j){
                switch(j){
                    case 0 : fileString = folder+"/"+std::to_string(i)+"/center.txt"; break;
                    case 1 : fileString = folder+"/"+std::to_string(i)+"/lateral.txt"; break;
                    case 2 : fileString = folder+"/"+std::to_string(i)+"/anterior.txt"; break;
                    case 3 : fileString = folder+"/"+std::to_string(i)+"/posterior.txt"; break;
                    case 4 : fileString = folder+"/"+std::to_string(i)+"/median.txt"; break;
                }


                std::ifstream file (fileString.c_str());
                  if (file.is_open())
                  {
                    switch(j){
                        case 0 : centerFiles.push_back(fileString);break;
                        case 1 : lateralFiles.push_back(fileString);break;
                        case 2 : anteriorFiles.push_back(fileString);break;
                    }

                    file.close();
                  }
                  else std::cout << "Unable to open file" << std::endl;
            }
        }

        std::shared_ptr<MERBundle> bundle = std::make_shared<MERBundle>();

        loadMeta(folder,bundle);

        bundle->addElectrode("lat", std::make_shared<MERElectrode>(lateralFiles));
        bundle->addElectrode("ant", std::make_shared<MERElectrode>(anteriorFiles));
        bundle->addElectrode("cen", std::make_shared<MERElectrode>(centerFiles));

        _bundles.insert(std::pair<std::string,std::shared_ptr<MERBundle>>(folder,bundle));

        return bundle;

}

void MERFileManager::loadMeta(const std::string& folder, std::shared_ptr<MERBundle> bundle){
    std::ifstream file (std::string(folder+"/meta.txt").c_str());
    std::string line;
    std::vector<std::string> lineSplit;
    int lineCounter = 0;
    if (file.is_open())
    {
        while ( std::getline (file,line) )
        {
            lineSplit = mocca::splitString<std::string>(line,' ');

            switch(lineCounter){
            case 0 :    {
                        if(lineSplit[1] == "right"){
                            bundle->setIsRightSide(true);
                        }else{
                            bundle->setIsRightSide(false);
                        }
                        }break;
            case 1 :    {
                        Core::Math::Vec3f target;
                        target.x = std::atof(lineSplit[1].c_str());
                        target.y = std::atof(lineSplit[2].c_str());
                        target.z = std::atof(lineSplit[3].c_str());
                        bundle->setTarget(target);
                        }break;
            case 2 :    {
                        Core::Math::Vec3f entry;
                        entry.x = std::atof(lineSplit[1].c_str());
                        entry.y = std::atof(lineSplit[2].c_str());
                        entry.z = std::atof(lineSplit[3].c_str());
                        bundle->setEntry(entry);
                        }break;
            }
            lineCounter++;
        }
    }else{
        std::cout << "[MERFileManager] Could not open meta file"<<std::endl;
    }
}


void MERFileManager::saveRecordings(std::shared_ptr<MERBundle> recording, const std::string& folder){
    std::string dir = "";

        #ifdef _WIN32
            dir = folder;
            mkdir(dir.c_str());
        #else
            dir = folder;
            mkdir(dir.c_str(), 0777);
        #endif

        for(int i = -10; i <= 5;++i){
#ifdef _WIN32
            mkdir(std::string(dir+"/"+std::to_string(i)).c_str());
#else
            mkdir(std::string(dir+"/"+std::to_string(i)).c_str(), 0777);
#endif
        }

        std::shared_ptr<MERElectrode>   currentElectrode;
        std::shared_ptr<MERData>        currentData;
        std::unique_ptr<std::ofstream>  file;
        std::string                     filename;

        for(int i = 0; i < 5;++i){
            switch(i){
                case 0 : currentElectrode = recording->getElectrode("cen"); filename = "center.txt"; break;
                case 1 : currentElectrode = recording->getElectrode("lat"); filename = "lateral.txt"; break;
                case 2 : currentElectrode = recording->getElectrode("ant"); filename = "anterior.txt"; break;
            }

            if(currentElectrode == nullptr) continue;

            for(int j = -10; j <= 5; j++){
                currentData = currentElectrode->getMERData(j);
                if(currentData == nullptr) continue;

                file = std::unique_ptr<std::ofstream>(new std::ofstream(std::string(dir+"/"+std::to_string(j)+"/"+filename)));

                if (file->is_open())
                {
                    for(int16_t s : currentData->getSignal(currentData->getRecordedSeconds())){
                        *(file.get()) << std::to_string(s)<< "\n";
                    }
                  file->close();
                }
            }

        }
}

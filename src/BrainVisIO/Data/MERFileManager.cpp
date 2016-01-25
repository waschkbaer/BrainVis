
#include "MERFileManager.h"
#include "MERBundle.h"
#include "MERData.h"
#include "MERElectrode.h"

#include <mocca/base/StringTools.h>

#include <iostream>
#include <fstream>
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
            for(int j = 3; j <= 5; ++j){
                fileString = folder+"/"+std::to_string(i)+"/s_c_"+std::to_string(j)+"_d_"+std::to_string(i)+".txt";

                std::ifstream file (fileString.c_str());
                  if (file.is_open())
                  {
                    switch(j){
                        case 3 : centerFiles.push_back(fileString);break;
                        case 4 : lateralFiles.push_back(fileString);break;
                        case 5 : anteriorFiles.push_back(fileString);break;
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

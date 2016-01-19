
#include "MERFileManager.h"
#include "MERBundle.h"
#include "MERData.h"
#include "MERElectrode.h"

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

        bundle->addElectrode("lat", std::make_shared<MERElectrode>(lateralFiles));
        bundle->addElectrode("ant", std::make_shared<MERElectrode>(anteriorFiles));
        bundle->addElectrode("cen", std::make_shared<MERElectrode>(centerFiles));

        _bundles.insert(std::pair<std::string,std::shared_ptr<MERBundle>>(folder,bundle));

        return bundle;

}

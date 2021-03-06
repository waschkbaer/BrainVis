#ifndef BRAINVIS_MERFileManager
#define BRAINVIS_MERFileManager

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace BrainVisIO{
namespace MERData{

class MERBundle;

class MERFileManager{
public:
    static MERFileManager& getInstance(){
        static MERFileManager instance;
        return instance;
    }

    const std::shared_ptr<MERBundle> openFolder(const std::string& folder);
    void saveRecordings(std::shared_ptr<MERBundle> recording, const std::string& folder);

protected:
    void loadMeta(const std::string& folder, std::shared_ptr<MERBundle> bundle);

private:
    std::map<std::string,std::shared_ptr<MERBundle>> _bundles;

    MERFileManager(){}
    ~MERFileManager(){}
};

}
}


#endif

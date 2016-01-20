#ifndef BRAINVIS_MERBundleManager
#define BRAINVIS_MERBundleManager

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace BrainVisIO{
namespace MERData{

class MERBundle;

class MERBundleManager{
public:
    static MERBundleManager& getInstance(){
        static MERBundleManager instance;
        return instance;
    }

    void addMERBundle(const std::string& identifier, const std::shared_ptr<MERBundle> bundle);
    void removeMERBundle(const std::string& identifier);

    const std::shared_ptr<MERBundle> getMERBundle(const std::string& identifier);

    const std::vector<std::string> getRegisteredBundles();

protected:

private:
    std::map<std::string,std::shared_ptr<MERBundle>> _bundles;

    MERBundleManager(){}
    ~MERBundleManager(){}
};

}
}


#endif

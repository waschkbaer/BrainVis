#ifndef BRAINVIS_MERBundle
#define BRAINVIS_MERBundle

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace BrainVisIO{
namespace MERData{

class MERElectrode;

class MERBundle{
public:
    MERBundle();
    MERBundle(const std::vector<std::string>& electrodeList);
    ~MERBundle();

    void addElectrode(const std::string& name, std::shared_ptr<MERElectrode> electrode = nullptr);

    const std::shared_ptr<MERElectrode> getElectrode(const std::string& name);

protected:

private:
    std::map<std::string,std::shared_ptr<MERElectrode>> _electrodes;

};

}
}


#endif

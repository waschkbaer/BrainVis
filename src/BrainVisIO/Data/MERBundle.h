#ifndef BRAINVIS_MERBundle
#define BRAINVIS_MERBundle

#include <vector>
#include <string>
#include <map>
#include <memory>

#include <core/Math/Vectors.h>

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

    void calculateElectrodePosition(Core::Math::Vec3f XAxis, Core::Math::Vec3f YAxis,
                                    Core::Math::Vec3f target, Core::Math::Vec3f entry);
    bool getIsActive() const;
    void setIsActive(bool isActive);

    Core::Math::Vec3f getTarget() const;
    void setTarget(const Core::Math::Vec3f &target);

    Core::Math::Vec3f getEntry() const;
    void setEntry(const Core::Math::Vec3f &entry);

    bool getIsRightSide() const;
    void setIsRightSide(bool isRightSide);

protected:
    void calculateDataPosition(std::shared_ptr<MERElectrode> electrode);
private:
    std::map<std::string,std::shared_ptr<MERElectrode>> _electrodes;
    bool                                                _isActive;

    Core::Math::Vec3f                                   _target;
    Core::Math::Vec3f                                   _entry;
    bool                                                _isRightSide;

};

}
}


#endif

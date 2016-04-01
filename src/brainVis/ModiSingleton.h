#ifndef MODSING
#define MODSING

enum Mode{
    TFEditor = 0,
    CameraMovement,
    CameraRotation,
    VolumePicking,
    TwoDScroll
};


class ModiSingleton{
public:
    static ModiSingleton& getInstance(){
         static ModiSingleton    instance;
         return instance;
    }

    Mode getActiveModeLeftClick() const {return _activeModeLeft;}
    Mode getActiveModeRightClick() const {return _activeModeRight;}
    void setActiveModeLeftClick(Mode m){_activeModeLeft = m;}
    void setActiveModeRightClick(Mode m){_activeModeRight = m;}

private:
    ModiSingleton():_activeModeLeft(Mode::CameraRotation),_activeModeRight(Mode::TFEditor){};
    ~ModiSingleton(){};

    Mode            _activeModeLeft;
    Mode            _activeModeRight;

};


#endif //modisingleton

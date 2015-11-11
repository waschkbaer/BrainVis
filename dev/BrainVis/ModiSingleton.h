#ifndef MODSING
#define MODSING

enum Mode{
    TFEditor = 0,
    CameraMovement,
    CameraRotation,
    VolumePicking
};


class ModiSingleton{
public:
    static ModiSingleton& getInstance(){
         static ModiSingleton    instance;
         return instance;
    }

    Mode getActiveMode() const {return _activeMode;}
    void setActiveMode(Mode m){_activeMode = m;}

private:
    ModiSingleton():_activeMode(Mode::TFEditor){};
    ~ModiSingleton(){};

    Mode            _activeMode;

};


#endif //modisingleton

#ifndef MUIHANDLER_H
#define MUIHANDLER_H

#include "../uis/widgets/renderwidget.h"
#include "../uis/widgets/mertool.h"
#include <mui/include/Gate.h>

#include <memory>
#include <thread>

class MuiHandler{
public:
    static MuiHandler& getInstance(){
         static MuiHandler    instance;
         return instance;
    }

    void connect(const std::string& hostname, const std::string& port);
    void run();
    void stop();

    void setRenderer(std::shared_ptr<RenderWidget> widget);
    void removeRenderer(std::shared_ptr<RenderWidget> widget);

    void setMerWidget(std::shared_ptr<MERTool> widget);
    void removeMerWidget(std::shared_ptr<MERTool> widget);
private:
    MuiHandler();
    ~MuiHandler();

    bool                            _connected;
    std::unique_ptr<std::thread>    _thread;

};

#endif // MUIHANDLER_H

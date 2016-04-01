#include "muihandler.h"

#include <atomic>

#include <renderer/DICOMRenderManager.h>

#define HANDLE_ERROR(val)                                                                                                                  \
    if (val != MUI_SUCCESS) {                                                                                                              \
        throw std::runtime_error(MUI_getError());                                                                                          \
    }

std::atomic<bool> exitFlag{ false };
void exitHandler(int s) {
    std::cout << "Exiting..." << std::endl;
    exitFlag = true;
}

MuiHandler::MuiHandler():
    _connected(false),
    _thread(nullptr){}
MuiHandler::~MuiHandler(){}

void MuiHandler::connect(const std::string& hostname, const std::string& port){
    stop();


    std::string typesPath = "./types/BrainVis.json";
    MUI_GateConfiguration config{{hostname.c_str(), port.c_str()}, "auto", "auto", ""};
    try {
        // configure the gate
        MUI_initialize(config);

        // register gate, application type, and application
        MUI_registerGate();
        MUI_registerApplicationTypes(typesPath.c_str());
        MUI_registerApplication("BrainVis", "Intraoperative DBS Tool");

        _thread = std::unique_ptr<std::thread>(new std::thread(&MuiHandler::run,this));
    } catch (const std::runtime_error& err) {

    }
}
void MuiHandler::run(){
    try{
        MUI_HEVENT event;
        long long count = 0;
        const uint16_t bufferSizeConst = 256;
        char buffer[bufferSizeConst];

        while (!exitFlag) {
            uint16_t bufferSize = bufferSizeConst;
            // try to receive the next event for Sample Application
            if (MUI_waitEvent(&event, "Intraoperative DBS Tool", 500, buffer, &bufferSize) != MUI_NO_EVENT) {
                ++count;
                std::cout << "#" << count << "\t\t" << buffer << std::endl;

                uint32_t numKeys;
                HANDLE_ERROR(MUI_eventNumKeys(event, &numKeys));
                if(std::string(buffer) == "[Renderer] Windowing"){
                    float position;
                    float gradient;
                    HANDLE_ERROR(MUI_eventKeyByIndex(event, 0, buffer, &bufferSize));
                    HANDLE_ERROR(MUI_eventGetFloatValue(event, buffer, &position));
                    HANDLE_ERROR(MUI_eventKeyByIndex(event, 1, buffer, &bufferSize));
                    HANDLE_ERROR(MUI_eventGetFloatValue(event, buffer, &gradient));

                    position = position + DicomRenderManager::getInstance().getPosition();
                    gradient = gradient + DicomRenderManager::getInstance().getGradient();
                    DicomRenderManager::getInstance().setSmoothStep(position,gradient);
                }


                /*for (uint32_t i = 0; i < numKeys; ++i) {
                    uint16_t bufferSize = bufferSizeConst;
                    HANDLE_ERROR(MUI_eventKeyByIndex(event, i, buffer, &bufferSize));
                    std::cout << "\t" << buffer << "\t";
                    MUI_EventValueType type;
                    HANDLE_ERROR(MUI_eventGetValueType(event, buffer, &type));
                    if (type == MUI_EventValueType::MUI_FLOAT) {
                        float val;
                        HANDLE_ERROR(MUI_eventGetFloatValue(event, buffer, &val));
                        std::cout << val << " (FLOAT)" << std::endl;
                    } else if (type == MUI_EventValueType::MUI_BOOL) {
                        bool val;
                        HANDLE_ERROR(MUI_eventGetBoolValue(event, buffer, &val));
                        std::cout << val << " (BOOL)" << std::endl;
                    } else if (type == MUI_EventValueType::MUI_INT32) {
                        int32_t val;
                        HANDLE_ERROR(MUI_eventGetIntValue(event, buffer, &val));
                        std::cout << val << " (INT32)" << std::endl;
                    } else if (type == MUI_EventValueType::MUI_STRING) {
                        uint16_t strValueSize;
                        HANDLE_ERROR(MUI_eventGetStringValueSize(event, buffer, &strValueSize));
                        std::unique_ptr<char[]> strBuffer(new char[strValueSize]);
                        HANDLE_ERROR(MUI_eventGetStringValue(event, buffer, strBuffer.get(), &strValueSize));
                        std::cout << strBuffer.get() << " (STRING)" << std::endl;
                    }
                }*/

                // destroy the received event when it's no longer needed
                HANDLE_ERROR(MUI_destroyEvent(event));
            }
        }
    }catch(const std::runtime_error& err){

    }

}
void MuiHandler::stop(){
    if(_thread != nullptr){
        exitHandler(1);
        _thread->join();
        _thread = nullptr;
        exitFlag=false;
   }
}

void MuiHandler::setRenderer(std::shared_ptr<RenderWidget> widget){}
void MuiHandler::removeRenderer(std::shared_ptr<RenderWidget> widget){}

void MuiHandler::setMerWidget(std::shared_ptr<MERTool> widget){}
void MuiHandler::removeMerWidget(std::shared_ptr<MERTool> widget){}

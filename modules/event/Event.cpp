#include    "Event.h"

namespace rv_xjtu_yangyan {

    void Event::setCurrentPID()
    {
        processId = getpid();
    }

    /*
     *参数：原始路径
     */
    void Event::setCurrentProcessName(std::string path)
    {
        std::string thisName = std::string(path);
        size_t pos = thisName.rfind('/');
        processName = thisName.substr(pos+1, thisName.length() - pos);
    }

    /*
     *读入函数名称
    */
    void Event::setFunctionName(std::string funcName)
    {
        functionName = funcName;
    }

    void Event::setFunctionArgs(ArgumentList &funcArgs)
    {
       functionArgs.clone(funcArgs);
    } 

    void Event::clone(Event &event)
    {
        this->processId = event.processId;
        this->processName = event.processName;
        this->functionName = event.functionName;
        this->functionArgs.clone(event.functionArgs);
    } 
}

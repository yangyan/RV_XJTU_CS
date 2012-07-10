#include    "Event.h"

namespace rv_xjtu_yangyan {

    void Event::setCurrentPID()
    {
        processId = getpid();
    }

    void Event::setCurrentProcessName(std::string path)
    {
        std::string thisName = std::string(path);
        size_t pos = thisName.rfind('/');
        processName = thisName.substr(pos+1, thisName.length() - pos);
    }



}

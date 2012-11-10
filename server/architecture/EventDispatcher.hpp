#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include  <map>

#include    "ProgramThread.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;
    class EventDispatcher
    {

        map<string, ProgramThread *> pn2pt_;
    };
    //end of class EventDispatcher

}

#endif

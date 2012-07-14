#ifndef EVENTPATH_CLASS
#define EVENTPATH_CLASS

#include  <list>
#include  <algorithm>
#include  <string>
#include  <functional>

#include  "Event.hpp"

namespace rv_xjtu_yangyan {
   
   class EventPath
   {
       public:
           std::vector<Event *> eventPath;

       public:
           ~EventPath();
           void push(const Event &eventItem);
           //void match(const EventPathGraph &epg);
           const std::string toString() const;
           void hello();
   };

}

namespace rv_xjtu_yangyan {

    //只能在本文件中使用的函数
    static void deleteEventInPath(Event *eventItem);

    EventPath::~EventPath()
    {
        for_each(eventPath.begin(),eventPath.end(), 
                deleteEventInPath);
    }

    static void deleteEventInPath(Event *eventItem)
    {
        delete eventItem;
    }

    void EventPath::push(const Event &eventItem)
    {
        Event *newEvent = new Event(eventItem);
        eventPath.push_back(newEvent);
    } 

    const std::string EventPath::toString() const
    {
        std::stringstream ss; 
        std::vector<Event *>::const_iterator iter;
        for(iter = eventPath.begin(); iter != eventPath.end() - 1; ++iter)
        {
            ss << (*iter)->functionName << "->";
        }
        ss << (*iter)->functionName;
        return ss.str();
    }

}

#endif

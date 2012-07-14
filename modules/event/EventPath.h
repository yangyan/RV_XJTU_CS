#ifndef EVENTPATH_CLASS
#define EVENTPATH_CLASS

#include  <list>
#include  <algorithm>
#include  <string>
#include  <functional>

#include  "Event.h"

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

#endif

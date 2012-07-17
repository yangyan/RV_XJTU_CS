#ifndef EVENTPATHHISTORY_CLASS
#define EVENTPATHHISTORY_CLASS

#include  <map>
#include  <algorithm>

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/set.hpp>

#include    "EventPath.hpp"
#include    "../solution/Solution.hpp"

namespace rv_xjtu_yangyan {

    class EventPathHistory {

        private:
            //删除一个路径的函数对象的定义
            struct deletePath{
                void operator()(std::pair<EventPath *, Solution *> p)
                {
                    delete p.first;
                    delete p.second;
                }
            };
            //比较map键值的函数
            struct PathComparer{
                bool operator()(EventPath *first, EventPath *second)
                {
                    if(first->hashValue < second->hashValue) {
                        return true;
                    } else if(first->hashValue == second->hashValue) {
                        return first->toString() < second->toString();
                    } else {
                        return false;
                    }
                }
            };


        public:
            //map中，first为事件路径，second为解决方案
            std::map<EventPath *, Solution *, PathComparer> eventPaths;

        public:
            EventPathHistory();
            ~EventPathHistory();
            void append(EventPath *, Solution *);
    };
}

namespace rv_xjtu_yangyan {

    EventPathHistory::EventPathHistory()
    {
    }

    EventPathHistory::~EventPathHistory()
    {
        for_each(eventPaths.begin(), eventPaths.end(), deletePath());
    }

    void EventPathHistory::append(EventPath *eventpath, Solution *solution)
    {
        EventPath *newEventPath = new EventPath();
        newEventPath->clone(*eventpath);
        Solution *newSolution = new Solution();
        newSolution->clone(*solution);
        //这里是单纯的插入，如果需要更新，再想办法
        eventPaths.insert(std::pair<EventPath *, Solution *>(newEventPath, newSolution));
    }



}
            
#endif

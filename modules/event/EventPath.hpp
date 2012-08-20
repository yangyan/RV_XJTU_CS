#ifndef EVENTPATH_CLASS
#define EVENTPATH_CLASS

#include  <list>
#include  <algorithm>
#include  <string>
#include  <functional>

#include  <boost/functional/hash.hpp>
#include  <boost/serialization/access.hpp>
#include  <boost/serialization/vector.hpp>

#include  "Event.hpp"

namespace rv_xjtu_yangyan {
    //EventPath在以下位置使用
    //1、在验证端记录
    //2、存储于路径历史中
   
   class EventPath
   {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & eventPath;
                   ar & hashValue;
               }

       public:
           EventPath();
           ~EventPath();
           void append(const Event &eventItem);
           //void match(const EventPathGraph &epg);
           const std::string toString() const;
           void clone(EventPath &eventpath);
           bool equalTo(const EventPath &);
           Event* operator[](size_t n);

       public:
           std::vector<Event *> eventPath;
           std::size_t          hashValue;
   };

}

namespace rv_xjtu_yangyan {

    //只能在本文件中使用的函数
    static void deleteEventInPath(Event *eventItem);

    EventPath::EventPath():hashValue(0){};

    EventPath::~EventPath()
    {
        for_each(eventPath.begin(),eventPath.end(), 
                deleteEventInPath);
    }

    static void deleteEventInPath(Event *eventItem)
    {
        delete eventItem;
    }

    void EventPath::append(const Event &eventItem)
    {
        Event *newEvent = new Event(eventItem);
        eventPath.push_back(newEvent);
        //计算哈希值
        boost::hash<std::string> string_hash;
        //这里，17可以换成其他数字
        hashValue = hashValue * 17 + string_hash(eventItem.functionName);
    } 

    bool EventPath::equalTo(const EventPath &eventPath)
    {
        if(this->toString() == eventPath.toString())
            return true;

        return false;
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

    void EventPath::clone(EventPath &eventpath)
    {
        std::vector<Event *>::const_iterator iter;
        for(iter = eventpath.eventPath.begin();
                iter != eventpath.eventPath.end(); ++iter)
        {
            Event *ep = new Event();
            ep->clone(*(*iter));
            eventPath.push_back(ep);
        }
        hashValue = eventpath.hashValue;
    }

    Event* EventPath::operator[](size_t n)
    {
        if(n >=  eventPath.size()) return NULL;
        return eventPath[n];
    }



}

#endif

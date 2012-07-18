#ifndef EVENTPATHHISTORY_CLASS
#define EVENTPATHHISTORY_CLASS

#include  <map>
#include  <algorithm>
#include  <fstream>
#include  <iostream>
#include  <sstream>

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/map.hpp>
#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

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
                bool operator()(EventPath *first, EventPath *second) const
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
            //输出一条路径
            struct OutPutPath{
                std::stringstream &ss;
                OutPutPath(std::stringstream &_ss):ss(_ss) { }
                void operator()(std::pair<EventPath *, Solution *> _p)
                {
                    ss << _p.first->toString() << std::endl;
                }
            };

       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & eventPaths;
               }


        public:
            //map中，first为事件路径，second为解决方案
            std::map<EventPath *, Solution *, PathComparer> eventPaths;

        public:
            EventPathHistory();
            ~EventPathHistory();
            //增加一个新的路径，若原路径存在，则更新
            void append(EventPath &ep, Solution &s);
            std::size_t size();
            Solution& getSolution(EventPath &ep);
            void storeToFile(const std::string file);
            void readFromFile(const std::string file);
            std::string toString();
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

    void EventPathHistory::append(EventPath &eventpath, Solution &solution)
    {
        EventPath *newEventPath = new EventPath();
        newEventPath->clone(eventpath);
        Solution *newSolution = new Solution();
        newSolution->clone(solution);
        //如果存在，则删除，不存在，以下操作不会删除任何元素
        eventPaths.erase(&eventpath);
        eventPaths.insert(std::pair<EventPath *, Solution *>(newEventPath, newSolution));
    }

    std::size_t EventPathHistory::size()
    {
        return eventPaths.size();
    }

    Solution& EventPathHistory::getSolution(EventPath &ep)
    {
        Solution *newSolution = new Solution();
        if(eventPaths.count(&ep))
        {
            newSolution->clone(*(eventPaths[&ep]));
        }
        return *newSolution;
    }

    void EventPathHistory::storeToFile(std::string file)
    {
        std::ofstream ofs(file.c_str(), std::ios::trunc);
        boost::archive::text_oarchive oa(ofs);
        oa << *this;
    }

    void EventPathHistory::readFromFile(const std::string file)
    {
        std::ifstream ifs(file.c_str());
        boost::archive::text_iarchive ia(ifs);
        ia >> *this;
    }

    std::string EventPathHistory::toString()
    {
        std::stringstream ss;
        for_each(eventPaths.begin(), eventPaths.end(), OutPutPath(ss));
        return ss.str();
    }
        

}
            
#endif

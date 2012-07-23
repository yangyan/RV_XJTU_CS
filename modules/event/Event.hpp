#ifndef EVENT_CLASS
#define EVENT_CLASS

#include  <string>
#include  <iostream>

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/string.hpp>

#include  <unistd.h>

#include    "ArgumentList.hpp"
#include    "EventTime.hpp"

namespace rv_xjtu_yangyan {
   
   class Event
   {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & processId;
                   ar & processName;
                   ar & functionName;
                   ar & functionArgs;
               }


       public:
           
           Event();
           Event(const Event &event);
           void setCurrentPID();
           void setCurrentProcessName(std::string path);
           void setFunctionName(std::string funcName);
           void setFunctionArgs(ArgumentList &funcArgs);
           void clone(const Event &event);
           long long timeSince(const Event &event);

       /*
        *从进程中获取的程序的信息
        */
       public:
           size_t       processId;
           std::string  processName;
           std::string  functionName;
           ArgumentList functionArgs;
           EventTime    birthTime;
           
   };

}

namespace rv_xjtu_yangyan {

    Event::Event()
    {
        EventTime birthTime = EventTime();
    }

    Event::Event(const Event &event)
    {
        this->clone(event);
    }

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

    void Event::clone(const Event &event)
    {
        this->processId = event.processId;
        this->processName = event.processName;
        this->functionName = event.functionName;
        this->functionArgs.clone(event.functionArgs);
        this->birthTime.clone(event.birthTime);
    } 

    long long Event::timeSince(const Event &event)
    {
        return this->birthTime.timeSince(event.birthTime);
    }
}
#endif

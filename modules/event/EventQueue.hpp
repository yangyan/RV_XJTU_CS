#ifndef EVENTQUEUE_CLASS
#define EVENTQUEUE_CLASS

#include  <queue>
#include  <algorithm>

#include  <boost/thread/mutex.hpp>

#include    "Event.hpp"

namespace rv_xjtu_yangyan 
{
   
   /*
    * 事件队列：是暂时存储到来事件的容器。
    * 由于在接受一个事件后，需要等待该事件被推理，
    * 然后才能继续运行被监控程序，因此，事件队列是
    * 不会有一个线程的多个事件的。
    * 1、如果被监控程序是单线程的，那么队列中最多只有一个事件；
    * 2、如果是多线程程序，那么队列中至多有线程数量个事件。
    */
   class EventQueue
   {
       public:
           std::queue<Event *> eventQueue;

       public:
           ~EventQueue();

           void push(const Event &eventItem);

           Event& pop();

           bool empty();
       private:
           boost::mutex mtx_;


   };

}

namespace rv_xjtu_yangyan 
{
    EventQueue::~EventQueue()
    {
        while(!eventQueue.empty()){
            delete eventQueue.front();
            eventQueue.pop();
        }
    }

    void EventQueue::push(const Event &eventItem)
    {
        //位于多线程环境下
        boost::mutex::scoped_lock lock(mtx_);

        Event *newEvent = new Event(eventItem);
        eventQueue.push(newEvent);
    }

    Event& EventQueue::pop()
    {
        //位于多线程环境下
        boost::mutex::scoped_lock lock(mtx_);

        Event *event = eventQueue.front();
        eventQueue.pop();
        return *event;
    }

    bool EventQueue::empty()
    {
        return eventQueue.empty();
    }

}

#endif

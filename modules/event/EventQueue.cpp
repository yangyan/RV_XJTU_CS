#include    "EventQueue.h"

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
        Event *newEvent = new Event(eventItem);
        eventQueue.push(newEvent);
    }

    Event& EventQueue::pop()
    {
        Event *event = eventQueue.front();
        eventQueue.pop();
        return *event;
    }

}

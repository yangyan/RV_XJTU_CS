#include    "event_api.h"

#include    "EventManager.hpp"

using namespace rv_xjtu_yangyan;

ManagerHandle event_manager_start()
{
    EventManager *em = new EventManager();
    em->run();
    return (ManagerHandle)em;
}

void create_new_event(ManagerHandle mh)
{
    EventManager *em = (EventManager *)mh;
    Event e = Event();
    em->syncAccept(e);
}

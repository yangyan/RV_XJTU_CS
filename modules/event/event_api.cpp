#include    "event_api.h"

#include    "EventManager.hpp"

#include  <stdarg.h>

using namespace rv_xjtu_yangyan;


ManagerHandle event_manager_start()
{
    EventManager *em = new EventManager();
    em->run();
    return (ManagerHandle)em;
}

void event_manager_stop(ManagerHandle mh)
{
    EventManager *em = (EventManager *)mh;
    em->safeStop();
}

EventHandle create_new_event()
{
    Event *e = new Event();
    e->setCurrentPID();
}

void event_set_func_name(EventHandle eh, const char *name)
{
    Event *e = (Event *) eh;
    e->setFunctionName(name);
}

void event_add_func_arg(EventHandle eh, const char *format, ...)
{
    Event *e = (Event *)eh;
    
    va_list pArgList;
    va_start(pArgList, format);
    int size = 0;
    size = vsnprintf(NULL, 0, format, pArgList);
    char *s = new char[size+1]();
    (void) vsnprintf(s, size+1, format, pArgList);
    va_end(pArgList);

    e->functionArgs.append(s);

    delete[] s;
}
    
void publish_event(ManagerHandle mh, EventHandle eh)
{
    EventManager *em = (EventManager *)mh;
    Event *e = (Event *) eh;
    em->syncAccept(*e);
}

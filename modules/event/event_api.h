#ifndef EVENT_API_H
#define EVENT_API_H

#include  <unistd.h>

typedef size_t * ManagerHandle;

extern ManagerHandle event_manager_start();

extern void create_new_event(ManagerHandle);

#endif




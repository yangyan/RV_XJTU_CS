#ifndef EVENT_API_H
#define EVENT_API_H

#include  <unistd.h>

#include  <iostream>
#include  <vector>

using namespace std;

typedef size_t * ManagerHandle;
typedef size_t * EventHandle;
typedef int (*FunctionHandle)(vector<string> &);

extern ManagerHandle event_manager_start(const char*);

extern void event_manager_add_solve_function(ManagerHandle, const char*, FunctionHandle);

extern void event_manager_stop(ManagerHandle);

extern EventHandle create_new_event();

extern void event_set_func_name(EventHandle, const char *);

extern void event_set_event_name(EventHandle, const char *);

extern void event_add_func_arg(EventHandle, const char *, ...);

void publish_event(ManagerHandle, EventHandle);

#endif




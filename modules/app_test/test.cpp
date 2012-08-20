#include    "headers.h"
#include  <iostream>

using namespace std;

ManagerHandle mh;

aspect RV_start{
    advice execution("% main(...)"):around(){
        mh = event_manager_start();
        tjp->proceed();
        event_manager_stop(mh);
    }
};

aspect Trace_func_a{
    advice execution("void func_a(int, float)"):before(){
        EventHandle eh = create_new_event();
        event_set_func_name(eh, "func_a");
        event_add_func_arg(eh, "%d", tjp->arg(1));
        event_add_func_arg(eh, "%f", tjp->arg(2));
        publish_event(mh, eh);
    }
};

aspect Trace_func_b{
    advice execution("void func_b(char *, int)"):before(){
        EventHandle eh = create_new_event();
        event_set_func_name(eh, "func_b");
        event_add_func_arg(eh, "%s", tjp->arg(1));
        event_add_func_arg(eh, "%d", tjp->arg(2));
        publish_event(mh, eh);
    }
};

void func_b(char *s, int c)
{
}

void func_a(int a, float b)
{
    func_b("hello world", 20);
    func_b("hello world", 20);
    func_b("hello world", 20);
}

int main(int argc, char *argv[])
{
    func_a(1,1.23);
    func_a(1,1.23);
    return 0;
}

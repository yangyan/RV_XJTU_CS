#include    "../../modules/event/event_api.h"

ManagerHandle mh;
EventHandle eh;

#include  <iostream>
#include  <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////
//自定义处理函数
///////////////////////////////////////////////////////////////////////
int exit(vector<string> &args)
{
    cout << "exit now!!!" << endl;
    return 0;
}


///////////////////////////////////////////////////////////////////////
//面向切面的定义
///////////////////////////////////////////////////////////////////////
aspect StartStopEventManager
{
    advice execution("% main(...)"):around()
    {
        mh = event_manager_start("test");
        event_manager_add_solve_function(mh, "exit", exit);


        eh = create_new_event();
        event_set_event_name(eh, "f_main_begin");
        event_set_func_name(eh, "f_main_begin");
        publish_event(mh, eh);

        tjp->proceed();

        eh = create_new_event();
        event_set_event_name(eh, "f_main_end");
        event_set_func_name(eh, "f_main_end");
        publish_event(mh, eh);

        event_manager_stop(mh);
    }

};

aspect MallocFree
{
    advice call("% malloc(...)"):after()
    {
        void **pp = tjp->result();
        cout << "mallocing..." << *pp << endl;

        eh = create_new_event();
        event_set_event_name(eh, "f_malloc");
        event_set_func_name(eh, "f_malloc");
        event_add_func_arg(eh, "%p", *pp);
        publish_event(mh, eh);
    }

    advice call("% free(...)"):after()
    {
        void **pp = tjp->arg<0>();
        cout << "freeing..." << *pp << endl;

        eh = create_new_event();
        event_set_event_name(eh, "f_free");
        event_set_func_name(eh, "f_free");
        event_add_func_arg(eh, "%p", *pp);
        publish_event(mh, eh);
    }
};

int *b()
{
    int *a = (int *)malloc(100);
    return a;
}
int main(int argc, char *argv[])
{
    int *a = b();
    free(a);
    return 0;
}

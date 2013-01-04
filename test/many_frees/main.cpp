#include    "../../modules/event/event_api.h"

ManagerHandle mh;
EventHandle eh;

#include  <iostream>
#include  <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////
//自定义处理函数
///////////////////////////////////////////////////////////////////////
bool skipfree = false;
int solution_skipfree(vector<string> &args)
{
    int *p = (int *)atoi(args.at(0).c_str());
    skipfree = true;
    cout << ">>> Thanks to SFM!!! Try to skip a free" << endl;
    return 0;
}

int solution_free(vector<string> &args)
{
    int *p = (int *)atoi(args.at(0).c_str());
    free(p);
    cout << ">>> Thanks to SFM!!! pointer " << p << " freed" << endl;
    return 0;
}

void often(int);

///////////////////////////////////////////////////////////////////////
//面向切面的定义
///////////////////////////////////////////////////////////////////////
aspect StartStopEventManager
{
    advice execution("% main(...)"):around()
    {
        mh = event_manager_start("test");
        event_manager_add_solve_function(mh, "solution_skipfree", solution_skipfree);
        event_manager_add_solve_function(mh, "solution_free", solution_free);


        eh = create_new_event();
        event_set_event_name(eh, "f_main@begin");
        publish_event(mh, eh);

        tjp->proceed();

        eh = create_new_event();
        event_set_event_name(eh, "f_main@end");
        publish_event(mh, eh);

        event_manager_stop(mh);
    }

};

aspect BeginEnd
{
    pointcut virtual functions() = 0;

    advice call(functions()):around()
    {
        eh = create_new_event();
        string beginEvent = "f_" + functionName + "@begin";
        event_set_event_name(eh, beginEvent.c_str());
        publish_event(mh, eh);

        cout << JoinPoint::argtype(0) << endl;
        tjp->proceed();

        eh = create_new_event();
        string endEvent = "f_" + functionName + "@end";
        event_set_event_name(eh, endEvent.c_str());
        publish_event(mh, eh);
    }

    string functionName;
};

aspect Often:public BeginEnd
{
    Often()
    {
        functionName = "often";
    }
    pointcut functions() = "void often(int)";
};


aspect MallocFree
{
    pointcut exclude() = !within("int solution_%(...)");

    advice call("% malloc(...)") && exclude():after()
    {
        void **pp = tjp->result();
        cout << "mallocing..." << *pp << endl;

        eh = create_new_event();
        event_set_event_name(eh, "f_malloc");
        event_set_func_name(eh, "f_malloc");
        event_add_func_arg(eh, "%d", *pp);
        publish_event(mh, eh);
    }

    advice call("% free(...)") && exclude():around()
    {
        void **pp = tjp->arg<0>();
        cout << "freeing..." << *pp << endl;

        eh = create_new_event();
        event_set_event_name(eh, "f_free");
        event_set_func_name(eh, "f_free");
        event_add_func_arg(eh, "%d", *pp);
        publish_event(mh, eh);

        if(skipfree == false)
            tjp->proceed();
    }
};

int *ex;

void often(int n)
{
    free(ex);
    int *a = (int *)malloc(100);
}



int main(int argc, char *argv[])
{
    ex = (int *)malloc(10);
    int i = 10;
    while(i--){
        sleep(1);
        often(0);
    }
    return 0;
}

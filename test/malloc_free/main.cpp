#include    "../../modules/event/event_api.h"

ManagerHandle mh;
EventHandle eh;

#include  <iostream>
#include  <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////
//自定义处理函数
///////////////////////////////////////////////////////////////////////
int solution_exit(vector<string> &args)
{
    int *p = (int *)atoi(args.at(0).c_str());
    free(p);
    cout << ">>> Thanks to SFM!!! pointer " << p << " freed" << endl;
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
        event_manager_add_solve_function(mh, "solution_exit", solution_exit);


        eh = create_new_event();
        event_set_event_name(eh, "f_main@begin");
        publish_event(mh, eh);

        tjp->proceed();

        eh = create_new_event();
        event_set_event_name(eh, "f_main@end");
        //event_set_func_name(eh, "f_main_end");
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
    pointcut functions() = "void often()";
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

    advice call("% free(...)") && exclude():after()
    {
        void **pp = tjp->arg<0>();
        cout << "freeing..." << *pp << endl;

        eh = create_new_event();
        event_set_event_name(eh, "f_free");
        event_set_func_name(eh, "f_free");
        event_add_func_arg(eh, "%d", *pp);
        publish_event(mh, eh);
    }
};

int *b(int n)
{
    if(n == 0) return 0;
    int *a = (int *)malloc(100);
    int *c = (int *)malloc(100);
    b(--n);
    free(a);
    return 0;
}

void often()
{
    b(3);
}


int main(int argc, char *argv[])
{
    while(true)
    {
        sleep(1);
        often();
    }
    return 0;
}

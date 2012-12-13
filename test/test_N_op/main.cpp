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
    cout << ">>> Thanks to SFM!!! pointer " << 1000 << " freed" << endl;
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

    advice call("% hello(...)") && exclude():after()
    {
        eh = create_new_event();
        event_set_event_name(eh, "f_hello");
        event_add_func_arg(eh, "%d", 1000);
        publish_event(mh, eh);
    }

    advice call("% world(...)") && exclude():after()
    {
        eh = create_new_event();
        event_set_event_name(eh, "f_world");
        event_add_func_arg(eh, "%d", 1000);
        publish_event(mh, eh);
    }
};

void hello()
{
}

void world()
{
}

void often()
{
    hello();
    world();
    hello();
}

int main(int argc, char *argv[])
{
    often();
    return 0;
}

#include    "../../modules/event/event_api.h"

#include  <iostream>

ManagerHandle mh;
EventHandle eh;

class A
{
    public:
        void *operator new(size_t size)
        {
            void *p = 0;
            return p;
        }
};

class B
{
    public:
};

/*
 *aspect StartAndStopSFM
 *{
 *    pointcut main() = "% main(...)";
 *
 *    advice execution(main()):before()
 *    {
 *        mh = event_manager_start("mainamainamainamaina");
 *        eh = create_new_event();
 *        event_set_event_name(eh, "f_main");
 *        publish_event(mh, eh);
 *    }
 *
 *    advice execution(main()):before()
 *    {
 *        eh = create_new_event();
 *        event_set_event_name(eh, "~f_main");
 *        publish_event(mh, eh);
 *        event_manager_stop(mh);
 *    }
 *};
 *
 *aspect NewAndDelete
 *{
 *    pointcut Ainit() = "A";
 *    pointcut main() = "% main(...)";
 *    advice construction(Ainit()) && within(main()):before()
 *    {
 *        eh = create_new_event();
 *        event_set_event_name(eh, "v_A");
 *        publish_event(mh, eh);
 *    }
 *};
 *
 */

aspect Trace
{
    pointcut virtual pc() = 0;
    advice execution(pc()):before()
    {
        std::cout << JoinPoint::signature() << std::endl;
    }
};

aspect FunctionTrace:public Trace
{
    pointcut pc() = "% ...::%(...)";
};

aspect ClassTrace
{
    pointcut pc() = "% ...::operator new";
    advice call(pc()):before()
    {
        std::cout << JoinPoint::signature() << std::endl;
    }
};

int main(int argc, char *argv[])
{
    A *a = new A;
    B *b = new B;
    return 0;
}

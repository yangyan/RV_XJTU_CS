#include    "../../modules/event/event_api.h"

ManagerHandle mh;
EventHandle eh;

#include  <iostream>
using namespace std;

int a(vector<string> &vs)
{
    cout << "这个函数成功运行了～～～Exit" << endl;
    return 0;
}
int Hello(vector<string> &vs)
{
    cout << "这个函数成功运行了～～～Hello" << endl;
    return 0;
}

int main(int argc, char *argv[])
{
    mh = event_manager_start("test");

    event_manager_add_solve_function(mh, "exit", a);
    event_manager_add_solve_function(mh, "hello", Hello);

    eh = create_new_event();
    event_set_event_name(eh, "v_b");
    publish_event(mh, eh);
/*
 *
 *    eh = create_new_event();
 *    event_set_event_name(eh, "~v_c");
 *    publish_event(mh, eh);
 */
    return 0;
}

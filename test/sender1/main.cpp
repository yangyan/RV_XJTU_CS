#include    "../../modules/event/event_api.h"

ManagerHandle mh;
EventHandle eh;

int main(int argc, char *argv[])
{
    mh = event_manager_start("mainamainamainamaina");
    eh = create_new_event();
    event_set_event_name(eh, "f_main");
    publish_event(mh, eh);

    eh = create_new_event();
    event_set_event_name(eh, "~f_main");
    publish_event(mh, eh);
    return 0;
}

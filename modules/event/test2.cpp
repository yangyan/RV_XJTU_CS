#include    "event_api.h"

ManagerHandle mh;

int main(int argc, char *argv[])
{
    mh = event_manager_start();
    create_new_event(mh);
    return 0;
}

#include    "headers.h"

ManagerHandle gmh;

int main(int argc, char *argv[])
{
    gmh = event_manager_start();
    create_new_event(gmh);
    return 0;
}

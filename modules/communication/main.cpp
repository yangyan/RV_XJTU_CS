#include    "Sender.hpp"

using namespace rv_xjtu_yangyan;
using namespace std;

int main(int argc, char *argv[])
{
    Sender c("localhost", "23451");
    Event e;
    e.setEventName(string("v_a"));
    e.setCurrentProcessName(string("test2"));
    c.write(e);
    return 0;
}

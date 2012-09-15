#include    "Sender.hpp"

using namespace rv_xjtu_yangyan;

int main(int argc, char *argv[])
{
    Sender c("localhost", "23451");
    c.write(*(new Event()));
    return 0;
}

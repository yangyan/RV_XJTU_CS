#include    "Receiver.hpp"

using namespace rv_xjtu_yangyan;

int main(int argc, char *argv[])
{
    Receiver r(23451);
    r.read();
    return 0;
}

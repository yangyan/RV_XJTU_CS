#include    "ProgramThreadFactory.hpp"

using namespace std;
using namespace rv_xjtu_yangyan;

int main(int argc, char *argv[])
{
    ProgramThreadFactory pt;
    pt.addFile("test.r");
    cout << pt.threads.size() << endl;
    pt.runAll();
    string input;
    while(true)
    {
        cin >> input;
        pt.threads[0]->inputEvent(input);
    }

    sleep(1000);
    return 0;
}

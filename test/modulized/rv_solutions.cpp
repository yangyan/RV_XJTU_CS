#include  <iostream>
#include  <vector>
#include  <cstdlib>
using namespace std;

bool skipfree = false;
int solution_skipfree(vector<string> &args)
{
    int *p = (int *)atoi(args.at(0).c_str());
    skipfree = true;
    cout << ">>> Thanks to SFM!!! Try to skip a free" << endl;
    return 0;
}

int solution_free(vector<string> &args)
{
    int *p = (int *)atoi(args.at(0).c_str());
    free(p);
    cout << ">>> Thanks to SFM!!! pointer " << p << " freed" << endl;
    return 0;
}

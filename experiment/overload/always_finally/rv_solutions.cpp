#include  <iostream>
#include  <vector>
#include  <cstdlib>

#define THIS_IS_CPP
#include  "rv_flags.h"

using namespace std;

int solution_check_fbe(vector<string> &args)
{
    int *a = (int *)atoi(args.at(0).c_str());
    int *b = (int *)atoi(args.at(1).c_str());
    int *ai = (int *)atoi(args.at(2).c_str());
    int *bi = (int *)atoi(args.at(3).c_str());
    int *ix = (int *)atoi(args.at(4).c_str());
    int *iy = (int *)atoi(args.at(5).c_str());

    if(a != b && ai == ix && bi == iy)
    {
        fbe_error = true;
    }
    return 0;
}

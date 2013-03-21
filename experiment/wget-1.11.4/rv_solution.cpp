#ifndef RV_SOLUTION_HPP
#define RV_SOLUTION_HPP

#include  <iostream>
#include  <cstdlib>
#include  <vector>
#include  <stdio.h>

using namespace std;

int solution_dofree(vector<string> &args)
{
    void *p= (void *)atoi(args.at(0).c_str());
    cout << "$$$$$--->free " << p << endl;

    free(p);
    return 0;
}


#endif

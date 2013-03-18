#ifndef RV_SOLUTION_HPP
#define RV_SOLUTION_HPP

#include  <iostream>
#include  <cstdlib>
#include  <vector>
#include  <stdio.h>

using namespace std;

int solution_doclose(vector<string> &args)
{
    void *fd= (void *)atoi(args.at(0).c_str());
    cout << "closing" << fd << endl;

    fclose((FILE *)fd);
    return 0;
}


#endif

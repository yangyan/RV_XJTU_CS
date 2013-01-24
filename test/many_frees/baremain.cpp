#include  <iostream>
#include  <cstdlib>
using namespace std;

void often()
{
    int *a = (int *)malloc(100);
    free(a);
    free(a);
    free(a);
}


int main(int argc, char *argv[])
{
    often();
    return 0;
}

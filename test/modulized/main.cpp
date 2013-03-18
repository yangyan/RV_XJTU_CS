#include  <iostream>
#include  <cstdlib>
using namespace std;

void often(int);


void often(int n)
{
    int *a = (int *)malloc(100);
    free(a);
    free(a);
    free(a);
}

int main(int argc, char *argv[])
{
    often(0);
    return 0;
}

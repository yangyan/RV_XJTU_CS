#include  <iostream>
#include  <cstdlib>
using namespace std;

int *b(int n)
{
    if(n == 0) return 0;
    int *a = (int *)malloc(100);
    int *c = (int *)malloc(100);
    b(--n);
    free(a);
    return 0;
}

void often()
{
    //b(3);
    int *a = (int *)malloc(100);
    free(a);
    free(a);
}


int main(int argc, char *argv[])
{
    often();
    return 0;
}

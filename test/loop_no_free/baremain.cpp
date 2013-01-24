#include  <iostream>
#include  <cstdlib>
using namespace std;

int *b;
void often()
{
    int *a = (int *)malloc(1024*1024);
    b = a;
}

int main(int argc, char *argv[])
{
    while(1)
    {
        sleep(1);
        often();
    }
    return 0;
}

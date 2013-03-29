#include  <iostream>
#include  <vector>
#include  <algorithm>
#include  <cstdlib>

using namespace std;

int total = 0;

void add(int n)
{
    total += n;
}

int main(int argc, char *argv[])
{
    vector<int> a, b;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    b.push_back(5);

    int loop = 1000;
    int choise;
    vector<int>::iterator iab, iae, ibb, ibe;
    iab = a.begin();
    iae = a.end();
    while(loop--)
    {
        srand(time(NULL));
        choise = rand()%4;
        switch(choise){
            case 0:
                a.begin();
                break;
            case 1:
                a.end();
                break;
            case 2:
                b.begin();
                break;
            case 3:
                b.end();
                break;
            case 4:
                a.begin();
                a.begin();
                for_each(iab , iae, add);
                break;
            case 5:
                a.end();
                a.end();
                for_each(iab, iae, add);
                break;
            case 6:
                a.begin();
                a.end();
                for_each(iab, iae, add);
                break;
            default:
                break;
        }
    }
    return 0;
}

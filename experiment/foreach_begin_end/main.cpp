#include  <iostream>
#include  <vector>
#include  <algorithm>

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

    a.begin();
    for_each(a.begin(), b.end(), add);

    cout << total << endl;

    for_each(a.begin(), a.end(), add);

    cout << total << endl;
    return 0;
}

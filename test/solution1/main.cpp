#include    "../../modules/solution/Result.hpp"
#include    "../../modules/solution/Solver.hpp"

#include  <iostream>
#include  <sstream>

using namespace rv_xjtu_yangyan;
using namespace std;
//打印两个数之和
int add(ArgumentList &al)
{
    int a, b, c;
    a = al[0].toInt();
    b = al[1].toInt();
    c = al[2].toInt();
    cout << a+b+c << endl;
}
int mul(ArgumentList &al)
{
    int a, b, c;
    a = al[0].toInt();
    b = al[1].toInt();
    c = al[2].toInt();
    cout << a*b*c*100 << endl;
}

int main(int argc, char *argv[])
{
    ArgumentList al;
    al.append(1);
    al.append(2);
    al.append(3);
    Solution s;
    s.functionName = "mul";
    s.type = Solution::FUNCTION;
    s.argumentList = al;
    Solver sl1;
    string add_str("add");
    string mul_str("mul");
    sl1.registerFunction(add_str, add);
    sl1.registerFunction(mul_str, mul);
    sl1.applySolution(s);
    return 0;
}

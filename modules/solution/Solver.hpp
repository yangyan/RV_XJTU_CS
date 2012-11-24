#ifndef SOLVER_CLASS
#define SOLVER_CLASS

#include  <vector>
#include  <iostream>

#include    "../../modules/event/ArgumentList.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    //这个类用于:
    //1、注册故障解决方法
    //2、解释Solution
    //3、调用相应的解决方法
    class Solver
    {
    public:
        typedef int (*solver_func)(ArgumentList &al);

    public:
        bool registerFunction(string &name, solver_func sf)
        {
            map<string, solver_func>::iterator iter; 
            iter = nfpair_.find(name);
            if(iter != nfpair_.end())
            {
                return false;//已经出现了
            }
            else
            {
                nfpair_[name] = sf;
                return true;
            }
        }
        
        void applySolution(Solution &s)
        {
            if(s.type == Solution::FUNCTION)
            {
                map<string, solver_func>::iterator iter;
                iter = nfpair_.find(s.functionName);
                if(iter != nfpair_.end())
                {
                    nfpair_[s.functionName](s.argumentList);
                }
                else
                {
                    //没有找到相应的解决方法，那么应该通知出错
                    cerr << "错误，没有找到所指出的函数：" << s.functionName << endl;
                    return;
                }
            }
            else
            {
                ///对于Solution为非FUNCTION类型的解决方法
                //......
                //......
                //......
                return;
            }
        }

    private:
        map<string, solver_func> nfpair_;//名字和函数对
    };
}
#endif

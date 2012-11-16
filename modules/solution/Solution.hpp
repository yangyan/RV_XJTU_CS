#ifndef SOLUTION_CLASS
#define SOLUTION_CLASS

#include  <boost/serialization/access.hpp>
#include  <iostream>

namespace rv_xjtu_yangyan{
    using namespace std;
    class Solution 
    {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & type;
               }
       public:
           void print()
           {
               switch(type)
               {
                   case CORRECT:
                       cerr << "CORRECT ";
                       break;
                   case NOT_GIVEN:
                       cerr << "NOT_GIVEN ";
                       break;
                   case NO_SOLUTION:
                       cerr << "NO_SOLUTION ";
                       break;
                   default:
                       ;
                       //do nothing
               }
           }

       public:
            enum TYPE
            {
                CORRECT,//表示推导正确，不需要解决
                NOT_GIVEN, 
                NO_SOLUTION
            };

            TYPE type;

        public:
            Solution();
            void clone(Solution &solution);
    };
}

namespace rv_xjtu_yangyan{

    Solution::Solution():type(NOT_GIVEN)
    {
    }

    void Solution::clone(Solution &solution)
    {
    }

}


#endif



#ifndef SOLUTION_CLASS
#define SOLUTION_CLASS

#include  <boost/serialization/access.hpp>

namespace rv_xjtu_yangyan{
    class Solution 
    {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
               }

        public:
            enum
            {
                NOT_GIVEN,
                NO_SOLUTION
            } type;

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



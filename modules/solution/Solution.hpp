#ifndef SOLUTION_CLASS
#define SOLUTION_CLASS

namespace rv_xjtu_yangyan{
    class Solution 
    {
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

    Solution::Solution:type(NOT_GIVEN)
    {
    }

    void Solution::clone(Solution &solution)
    {
    }

}


#endif



#ifndef RESULT_CLASS
#define RESULT_CLASS

#include  <vector>
#include  <iostream>

#include  <boost/asio.hpp>

#include    "Solution.hpp"
#include    "Solver.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;
    class Result
    {
    private:
        //串行化
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive &ar, const unsigned int version)
            {
                ar & solutions_;
            }
    public:
        /*
         *Result(boost::asio::ip::tcp::socket &s)
         *{
         *}
         */
        Result()
        {
        }

        void clear()
        {
            solutions_.clear();
        }

        void pushBackSolution(Solution &s)
        {
            solutions_.push_back(s);
        }

        void print()
        {
            for(vector<Solution>::iterator it = solutions_.begin();
                    it != solutions_.end(); it++)

            {
                (*it).print();
            }
            cout << endl;
        }

        void solveAll(Solver &s)
        {
            for(vector<Solution>::iterator it = solutions_.begin();
                    it != solutions_.end(); it++)
            {
                s.applySolution(*it);
            }
        }

    public:
        //boost::asio::ip::tcp::socket socket;

    private:
        vector<Solution> solutions_;
    };
}
#endif

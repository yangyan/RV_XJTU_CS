#ifndef RESULT_HPP
#define RESULT_HPP

#include  <vector>
#include  <iostream>

#include  <boost/asio.hpp>

#include    "Solution.hpp"

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

    public:
        //boost::asio::ip::tcp::socket socket;

    private:
        vector<Solution> solutions_;
    };
}
#endif

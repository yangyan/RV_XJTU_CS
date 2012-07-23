#ifndef EVENTTIME_CLASS
#define EVENTTIME_CLASS

#include  <iostream>

#define BOOST_CHRONO_HEADER_ONLY
#include  <boost/chrono/include.hpp>

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/string.hpp>

namespace rv_xjtu_yangyan
{
    class EventTime 
    {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & time;
               }

        public:
            EventTime()
            {
                std::time_t start(0);
                boost::chrono::nanoseconds nano = boost::chrono::system_clock::now()
                    - boost::chrono::system_clock::from_time_t(start);
                time = nano.count();
            }

            long long timeSince(const EventTime &_et)
            {
                return time - _et.time;
            }

            //这个函数应该没什么用
            long long microSecondsSince(EventTime &_et)
            {
                boost::chrono::nanoseconds start(_et.time);
                boost::chrono::nanoseconds end(time);
                boost::chrono::microseconds us = 
                    boost::chrono::duration_cast<boost::chrono::microseconds>(end - start);
                return us.count();
            }

            void clone(const EventTime &_et)
            {
                this->time = _et.time;
            }


        public:
            //事件产生的绝对时间,纳秒
            long long time;
    };

    //比较操作
    bool operator<(const EventTime &lhs, const EventTime &rhs)
    {
        return (lhs.time < rhs.time)?true:false;
    }

}
#endif

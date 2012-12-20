#ifndef SEMAPHORE_CLASS
#define SEMAPHORE_CLASS

#include  <iostream>

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

namespace rv_xjtu_yangyan
{
    class semaphore
    {
        private:
            boost::mutex mutex_;
            boost::condition_variable condition_;
            unsigned long count_;

        public:
            semaphore()
                : count_()
            {}

            void notify()
            {
                boost::mutex::scoped_lock lock(mutex_);
                ++count_;
                //std::cout << count_ << std::endl;
                condition_.notify_one();
            }

            void wait()
            {
                boost::mutex::scoped_lock lock(mutex_);
                while(!count_)
                    condition_.wait(lock);
                --count_;
            }
    };
}

#endif

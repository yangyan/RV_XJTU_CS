#ifndef EVENTMANAGER_CLASS
#define EVENTMANAGER_CLASS

#include "EventPathHistory.hpp"
#include "EventQueue.hpp"
#include "Event.hpp"

#include  <boost/bind.hpp>
#include  <boost/thread/thread.hpp>
#include  <boost/thread/mutex.hpp>
#include  <boost/thread/condition_variable.hpp>
#include  <boost/chrono.hpp>

#include    "../basic/semaphore.hpp"

namespace rv_xjtu_yangyan
{
    class EventManager 
    {
        private:
            struct EventComparer{
                bool operator()(Event *first, Event *second) const
                {
                    return ((first->birthTime < second->birthTime)||
                            (first->processId < second->processId))?
                        true:false;
                }
            };
        /*
         *对于事件的处理，可以采取同步或者异步的模式：
         *一、同步模式：就是事件到达后，必须等待故障管理系统
         *    处理得出结果后，程序才能继续运行。
         *二、异步模式：程序的运行和故障管理系统，可以并行运
         *    行。
         *目前，我们采用同步模式，异步模式以后研究。
         */
        private:
            EventPathHistory *eventHistory_;
            EventQueue *eventQueue_;
            std::set<Event *, EventComparer> processedEvents_;

            boost::mutex mutexProcessed_;
            semaphore semNewEvents_;
            semaphore semProcessedEvents_;
            semaphore semStop_;
            boost::thread *pRunThread_;
            bool stop_;
            bool running_;
        public:
            EventManager();
            //以历史文件为参数的初始化
            EventManager(const char *_file);
            ~EventManager();
            void setHistoryFile(const char *_file);

            void syncAccept(Event &_event);
            void run();
            void safeStop();
        private:
            void waitProcessed_(Event &_e);
            void storeAndNotifyProcessed_(const Event&_e);
            void run_();
    };
}

namespace rv_xjtu_yangyan
{
    void EventManager::run()
    {
        pRunThread_= new boost::thread(boost::bind(&EventManager::run_, this));
        pRunThread_->detach();
    }

    //主要执行过程
    void EventManager::run_()
    {
        //当收到停止信号，并且所有事件都被处理完毕，就可以结束了
        while(!(stop_ == true && (*eventQueue_).empty()))
        {
            semNewEvents_.wait();
            if((*eventQueue_).empty())
            {
                //可以结束了
                continue;
            }
            //对所有队列中的事件进行处理
            //.........
            //.........
            //.........
            //.........
            //.........
            storeAndNotifyProcessed_((*eventQueue_).pop());
            std::cout << "^_^" << std::endl;
        }
        semStop_.notify();
    }

    //保证这个函数是在程序最后运行
    void EventManager::safeStop()
    {
        stop_ = true;
        semNewEvents_.notify();
        semStop_.wait();
    }

    EventManager::EventManager():running_(false),stop_(false)
    {
        eventHistory_ = new EventPathHistory();
        eventQueue_ = new EventQueue();
    }

    EventManager::EventManager(const char *_file):running_(false),stop_(false)
    {
        eventHistory_ = new EventPathHistory();
        (*eventHistory_).setStoreFile(_file);
        eventQueue_ = new EventQueue();
    }

    EventManager::~EventManager()
    {
        //删除未处理的事件
        delete eventQueue_;
        delete eventHistory_;
    }

    void EventManager::setHistoryFile(const char *_file)
    {
        (*eventHistory_).setStoreFile(_file);
    }

    void EventManager::syncAccept(Event &_event)
    {
        //将事件放入事件队列中
        (*eventQueue_).push(_event);
        //通知处理程序处理
        semNewEvents_.notify();
        //等待事件被处理
        waitProcessed_(_event);
    }

    //这个函数，在已完成事件集中，寻找_e，如果找到，则删除之；
    //如果找不到，那么就继续阻塞
    void EventManager::waitProcessed_(Event &_e)
    {
        while(true)
        {
            size_t count;
            semProcessedEvents_.wait();
            {
                mutexProcessed_.lock();
                count = processedEvents_.count(&_e);
                mutexProcessed_.unlock();
            }
            if(0 == count) {
                semProcessedEvents_.notify();
            } else {
                break;
            }
        }
        {
            mutexProcessed_.lock();
            Event *e = *(processedEvents_.find(&_e));
            //从事件set中删除
            processedEvents_.erase(&_e);
            //释放事件
            delete e;
            mutexProcessed_.unlock();
        }
    }

    void EventManager::storeAndNotifyProcessed_(const Event&_e)
    {
        Event *e = new Event(_e);
        {
            mutexProcessed_.lock();
            processedEvents_.insert(e);
            mutexProcessed_.unlock();
        }
        semProcessedEvents_.notify();
    }

}


#endif

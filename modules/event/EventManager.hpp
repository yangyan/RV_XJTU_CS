#ifndef EVENTMANAGER_CLASS
#define EVENTMANAGER_CLASS

#include  <boost/bind.hpp>
#include  <boost/thread/thread.hpp>
#include  <boost/thread/mutex.hpp>
#include  <boost/thread/condition_variable.hpp>
#include  <boost/chrono.hpp>

namespace rv_xjtu_yangyan
{
    class EventManager 
    {
        private:
            struct EventComparer{
                bool operator()(Event *first, Event *second) const
                {
                    std::cout << first->birthTime.time << std::endl;
                    std::cout << second->birthTime.time << std::endl;
                    return ((first->birthTime < second->birthTime)&&
                            (first->processId == second->processId))?
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
            boost::mutex mutexEventSet_;
            boost::condition_variable cvEventSet_;
            boost::mutex mutexNewEvent_;
            boost::condition_variable cvNewEvent_;
            boost::mutex mutexStop_;
            boost::condition_variable cvStop_;
            boost::thread *pRunThread_;
            bool stop_;
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
        boost::mutex::scoped_lock lock(mutexNewEvent_);
        //当收到停止信号，并且所有事件都被处理完毕，就可以结束了
        while(!(stop_ == true && (*eventQueue_).empty()))
        {
            cvNewEvent_.wait(lock);
            //对所有队列中的事件进行处理
            if((*eventQueue_).empty())
            {
                std::cout << "队列空了" << std::endl;
                continue;
            }
            storeAndNotifyProcessed_((*eventQueue_).pop());
            std::cout << "this is one event" << std::endl;
        }
        cvStop_.notify_one();
    }

    //保证这个函数是在程序最后运行
    void EventManager::safeStop()
    {
        boost::mutex::scoped_lock lock(mutexStop_);
        stop_ = true;
        cvNewEvent_.notify_one();
        cvStop_.wait(lock);
    }

    EventManager::EventManager():running_(false),stop_(false)
    {
        eventHistory_ = new EventPathHistory();
        eventQueue_ = new EventQueue();
        //run_还没有运行就绪
        mutexRunReady_.lock();
    }

    EventManager::EventManager(const char *_file):running_(false),stop_(false)
    {
        eventHistory_ = new EventPathHistory();
        (*eventHistory_).setStoreFile(_file);
        eventQueue_ = new EventQueue();
        //run_还没有运行就绪
        mutexRunReady_.lock();
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
        cvNewEvent_.notify_one();
        //等待事件被处理
        waitProcessed_(_event);
    }

    //这个函数，在已完成事件集中，寻找_e，如果找到，则删除之；
    //如果找不到，那么就继续阻塞
    void EventManager::waitProcessed_(Event &_e)
    {
        boost::mutex::scoped_lock lock(mutexEventSet_);
        while(processedEvents_.count(&_e) == 0)
        {
            cvEventSet_.wait(lock);
        }
        Event *e = *(processedEvents_.find(&_e));
        //从事件集中删除
        processedEvents_.erase(&_e);
        //释放事件
        delete e;
    }

    void EventManager::storeAndNotifyProcessed_(const Event&_e)
    {
        Event *e = new Event(_e);
        processedEvents_.insert(e);
        cvEventSet_.notify_all();
    }

}


#endif

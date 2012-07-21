#ifndef EVENTMANAGER_CLASS
#define EVENTMANAGER_CLASS

namespace rv_xjtu_yangyan
{
    class EventManager 
    {
        /*
         *对于事件的处理，可以采取同步或者异步的模式：
         *一、同步模式：就是事件到达后，必须等待故障管理系统
         *    处理得出结果后，程序才能继续运行。
         *二、异步模式：程序的运行和故障管理系统，可以并行运
         *    行。
         *目前，我们采用同步模式，异步模式以后研究。
         */
        private:
            EventPathHistory eventHistory_;
            EventQueue  eventQueue_;
        public:
            EventManager();
            //以历史文件为参数的初始化
            EventManager(const char *_file);
            ~EventManager();
            void setHistoryFile(const char *_file);

    };
}

namespace rv_xjtu_yangyan
{
    EventManager::EventManager()
    {
        eventHistory_ = EventPathHistory();
        eventQueue_ = EventQueue();
    }

    EventManager::EventManager(const char *_file)
    {
        eventHistory_ = EventPathHistory();
        eventHistory_.setHistoryFile(_file);
        eventQueue_ = EventQueue();
    }

    EventManager::~EventManager()
    {
    }

    void EventManager::setHistoryFile(const char *_file)
    {
        eventQueue_.setHistoryFile(_file);
    }

    void EventManager::syncAccept(Event &_event)
    {
        //将事件放入事件队列中
        eventQueue_.push(_event);
        //等待事件被处理
        waitProcessed(_event);
    }

}


#endif

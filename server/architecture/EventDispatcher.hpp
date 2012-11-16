#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include  <map>

#include    "../../modules/event/Event.hpp"
#include    "../../modules/solution/Result.hpp"

#include    "ProgramThread.hpp"
#include    "ProgramThreadFactory.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;
    class EventDispatcher
    {
    public:
        EventDispatcher()
        {
        }

        //清空所有数据，重新设置，因此，需要factory一次性设置好
        void setThreads(ProgramThreadFactory &factory)
        {
            pn2pt_.clear();
            for(vector<ProgramThread *>::iterator it = factory.threads.begin();
                    it != factory.threads.end(); it++)
            {
                pn2pt_[(*it)->programName] = (*it);
            }
        }

        //直接传入程序名称和事件名称
        /*
         *void inputEvent(string program, string event)
         *{
         *    if(pn2pt_.find(program) != pn2pt_.end())
         *    {
         *        pn2pt_[program]->inputEvent(event);
         *    }
         *    else
         *    {
         *        cerr << "错误:" << program  << "不存在" << endl;
         *    }
         *}
         */

        //传入事件
        void inputEvent(Event &event, Result &r)
        {
            //首先要确保这个程序存在
            if(pn2pt_.find(event.processName) != pn2pt_.end())
            {
                pn2pt_[event.processName]->inputEvent(event, r);
            }
            else
            {
                cerr << "错误:" << event.processName << "不存在" << endl;
            }
        }

        map<string, ProgramThread *> pn2pt_;
    };
    //end of class EventDispatcher

}

#endif

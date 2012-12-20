#ifndef PROGRAMTHREAD_CLASS
#define PROGRAMTHREAD_CLASS

#include  <vector>
#include  <queue>

#include  <boost/thread/thread.hpp>
#include  <boost/uuid/uuid.hpp>
#include  <boost/uuid/uuid_io.hpp>
#include  <boost/uuid/random_generator.hpp>

#include    "../../modules/event/Event.hpp"
#include    "../../modules/solution/Result.hpp"
#include    "../../modules/basic/semaphore.hpp"
#include    "../../modules/solution/Solution.hpp"

#include    "../automata/automata.hpp"
#include    "../automata/reasonning.hpp"
#include    "InterestEvents.hpp"
#include    "Automata.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    class  ProgramThread
    {
    public:
        //内部类


        ///////////////////////////////////////////////////////////////////////
        //函数
        ///////////////////////////////////////////////////////////////////////

        ProgramThread(string pname)
            :programName(pname)
        {
        }

        //在这个线程中插入一个自动机（非具体）
        void insertAutomata(automata_rule *ar)/*{{{*/
        {
            for(vector<automata_type *>::iterator it = ar->automatas.begin();
                    it != ar->automatas.end(); it++)
            {
                Automata a = Automata(ar->rule_name, *it);
                automatas_.push_back(a);
            }

        }/*}}}*/

        //对输入事件进行推理
        void run()/*{{{*/
        {
            pReasonningThread = new boost::thread(boost::bind(&ProgramThread::reasonning_, this));
            pReasonningThread->detach();
        }/*}}}*/

        //使用Event事件
        void inputEvent(Event &event, Result &r)/*{{{*/
        {
            cout << event.eventName << endl;
            //inputs_.push(event.eventName);
            inputs_.push(event);
            resultptr_ = &r;
            semInput_.notify();
            semFinish_.wait();
        }/*}}}*/

    private:
        void reasonning_()/*{{{*/
        {
            while(true)
            {
                /*
                 *需要添加一些机制使它停下来
                 *比如说信号之类的
                 */
                semInput_.wait();
                //string newEvent = inputs_.front();
                Event newEvent = inputs_.front();
                inputs_.pop();
                //查找本自动机中所有的自动机
                for(vector<Automata>::iterator it = automatas_.begin();
                        it != automatas_.end(); it++)
                {
                    Automata &A = (*it);
                    A.getSolutionAll(*resultptr_, newEvent);
                }
                semFinish_.notify();
            }
        }/*}}}*/



    public:
        string programName;

    private:
        vector<Automata> automatas_;
        //queue<string> inputs_;
        queue<Event> inputs_;
        semaphore semInput_;
        semaphore semFinish_;
        boost::thread *pReasonningThread;
        Result *resultptr_;
    };

}
#endif

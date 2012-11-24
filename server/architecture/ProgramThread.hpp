#ifndef PROGRAMTHREAD_HPP
#define PROGRAMTHREAD_HPP

#include  <vector>
#include  <queue>

#include  <boost/thread/thread.hpp>

#include    "../../modules/event/Event.hpp"
#include    "../../modules/solution/Result.hpp"
#include    "../../modules/basic/semaphore.hpp"
#include    "../../modules/solution/Solution.hpp"

#include    "../automata/automata.hpp"
#include    "../automata/reasonning.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    class  ProgramThread
    {
    public:
        //内部类
        class InterestEvents
        {
        public:
            InterestEvents(automata_type *at)
            {
                fillEvents(events_, at);
                statusInit();
            }

            //检查是否包含某个事件
            bool hasMember(const string &event)
            {
                for(vector<string>::iterator it = events_.begin();
                        it != events_.end(); it++)
                {
                    if(event == (*it) 
                            || (event.at(0) == '~' && event.substr(1) == (*it)))
                    {
                        return true;
                    }
                }
                return false;
            }

            //设置一个事件的状态
            void setStatus(string event, bool status)
            {
                if(hasMember(event))
                {
                    status_[event] = status;
                }
                else
                {
                    cerr << "插入的事件不存在" << endl;
                }
            }
            void print()
            {
                for(map<string, bool>::iterator it = status_.begin();
                        it != status_.end(); it++)
                {
                    if((*it).second == true)
                    {
                        cout << ((*it).first) << " ";
                    }
                    else
                    {
                        cout << ("~" + (*it).first) << " ";
                    }
                }
                cout << endl;
            }
            //通过解析event的形式来判断状态
            void setStatus(string event)
            {
                if(event.at(0) != '~')
                {
                    if(hasMember(event))
                    {
                        status_[event] = true;
                    }
                }
                else
                {
                    if(hasMember(event.substr(1)))
                    {
                        status_[event.substr(1)] = false;
                    }
                }
            }

            //获取事件，以～e的形式
            vector<string> getEvents()
            {
                vector<string> rv;
                for(map<string, bool>::iterator it = status_.begin();
                        it != status_.end(); it++)
                {
                    if((*it).second == true)
                    {
                        rv.push_back((*it).first);
                    }
                    else
                    {
                        rv.push_back("~" + (*it).first);
                    }
                }
                return rv;
            }

        private:
            //从自动机中获取所有事件，不包含事件的状态
            void fillEvents(vector<string> &es, automata_type *at)
            {
                get_events_from_at((vector<string> &) es)(at);
            }
            //设置所有事件的状态
            void statusInit()
            {
                for(vector<string>::iterator it = events_.begin();
                        it != events_.end(); it++)
                {
                    status_[*it] = false;
                }
            }

        private:
            vector<string> events_;
            map<string, bool> status_;
        };

        class Automata 
        {
        public:
            Automata(string rname, automata_type *at)
                :automata(at), ruleName(rname)
            {
                next_oc = get_or_collection(at);
            }

            //检测自动机当前状态是否符合输入的事件，并获取下一步时间的值
            Solution is_satisfy(InterestEvents &ie)
            {
                cout << "当前集合为：";
                or_collection_printer(next_oc);
                cout << "输入事件为：";
                ie.print();

                vector<string> nowEvents;
                next_oc = or_satisfy_events(next_oc, ie.getEvents());
                //这里，我们认为如果没有下一步的可接受集合，就说明本次推理失败，
                //虽然这个观点对于v_a这种单步表达式是错误的，但是，对于一个能够连续
                //运行的程序，不能看单步的结果，因此使用size（）==0这种方法判断有
                //一定的合理性
                if(next_oc->ands.size() == 0)
                {
                    Solution s;
                    s.type = Solution::FUNCTION;
                    s.functionName = automata->solution;
                    return s;
                }
                else
                {
                    Solution s;
                    s.type = Solution::CORRECT;
                    return s;
                }
            }
        public:
            string ruleName;
            automata_type *automata;
            or_collection *next_oc;
        };

        ///////////////////////////////////////////////////////////////////////
        //函数
        ///////////////////////////////////////////////////////////////////////

        ProgramThread(string pname)
            :programName(pname)
        {
        }

        //在这个线程中插入一个自动机
        void insertAutomata(automata_rule *ar)
        {
            for(vector<automata_type *>::iterator it = ar->automatas.begin();
                    it != ar->automatas.end(); it++)
            {
                InterestEvents *ie = new InterestEvents(*it);
                Automata *a = new Automata(ar->rule_name, *it);
                automatas_.push_back(make_pair<InterestEvents *, Automata *>(ie, a));
            }

        }

        //对输入事件进行推理
        void run()
        {
            pReasonningThread = new boost::thread(boost::bind(&ProgramThread::reasonning_, this));
            pReasonningThread->detach();
        }

        //输入事件
        //之用字符串事件
        /*
         *void inputEvent(string event)
         *{
         *    inputs_.push(event);
         *    semInput_.notify();
         *}
         */

        //使用Event事件
        void inputEvent(Event &event, Result &r)
        {
            cout << event.eventName << endl;
            inputs_.push(event.eventName);
            resultptr_ = &r;
            semInput_.notify();
            semFinish_.wait();
        }

    private:
        void reasonning_()
        {
            while(true)
            {
                /*
                 *需要添加一些机制使它停下来
                 *比如说信号之类的
                 */
                semInput_.wait();
                string newEvent = inputs_.front();
                inputs_.pop();
                for(vector<pair<InterestEvents *, Automata *> >::iterator it = automatas_.begin();
                        it != automatas_.end(); it++)
                {
                    InterestEvents *pIE = (*it).first;
                    Automata *pA = (*it).second;
                    if(pIE->hasMember(newEvent))
                    {
                        //推理过程
                        pIE->setStatus(newEvent);
                        Solution s;
                        s = pA->is_satisfy(*pIE);
                        if(s.type == Solution::CORRECT)
                        {
                            resultptr_->pushBackSolution(s);
                            cout << "推理成功，进入下一步" << endl;
                        }
                        else
                        {
                            resultptr_->pushBackSolution(s);
                            cout << "推理失败，停止" << endl;
                        }

                    }
                }
                semFinish_.notify();
            }
        }



    public:
        string programName;

    private:
        vector<pair<InterestEvents *, Automata *> > automatas_;
        queue<string> inputs_;
        semaphore semInput_;
        semaphore semFinish_;
        boost::thread *pReasonningThread;
        Result *resultptr_;
    };

}
#endif

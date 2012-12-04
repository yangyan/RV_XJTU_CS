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

            //检查是不是开头事件
            bool isBegin(const string &event)
            {
                if(event == beginEvent_)
                    return true;
                else 
                    return false;
            }
            //检查是不是结尾事件
            bool isEnd(const string &event)
            {
                if(event == endEvent_)
                    return true;
                else
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
                //从自动机公式中获得事件
                get_events_from_at((vector<string> &) es)(at);
                //从scope中获得事件，这个事件为函数事件也就是f_xxxx
                if(at->scope->samelevel == true)
                {
                    //对于within中的事件，需要添加额外的后缀来区别开头和结尾
                    beginEvent_ = (at->scope->begin->event_name) + "_begin";
                    endEvent_ = (at->scope->end->event_name) + "_end";
                }
                else
                {
                    beginEvent_ = (at->scope->begin->event_name);
                    endEvent_ = (at->scope->end->event_name);
                }

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
            string beginEvent_;
            string endEvent_;
            map<string, bool> status_;
        };

        class Automata 
        {
        public:
            Automata(string rname, automata_type *at)
                :automata(at), ruleName(rname)
            {
                //next_oc = get_or_collection(at);
            }

            void newAutomata()
            {
                or_collection *oc = get_or_collection(automata);
                oc_level.push_back(pair<or_collection *, int>(oc, 0));
            }

            void deleteAutomata()
            {
                for(vector<pair<or_collection *, int> >::iterator it = oc_level.begin();
                        it != oc_level.end();)//此处不需要++
                {
                    if((*it).second == 0) it = oc_level.erase(it);
                    else it++;
                }
            }

            void getSolutionAll(Result &rslt, InterestEvents &ie, bool isEnd)
            {
                for(vector<pair<or_collection *, int> >::iterator it = oc_level.begin();
                        it != oc_level.end(); it++)
                {
                    Solution s = is_satisfy((*it).first, ie, isEnd);
                    rslt.pushBackSolution(s);
                }
            }
            //检测自动机当前状态是否符合输入的事件，并获取下一步时间的值
            Solution is_satisfy(or_collection *&oc, InterestEvents &ie, bool isEnd)
            {
                cout << "当前集合为：";
                or_collection_printer(oc);
                cout << "输入事件为：";
                ie.print();

                //对于无法推理下去的集合，后面的解决方案都是ignore
                if(oc->ands.size() == 0)
                {
                    Solution s;
                    s.type = Solution::NO_SOLUTION;
                    cout << "无法推理下去了，忽略" << endl;
                    return s;
                }

                vector<string> nowEvents;
                bool terminalResult;
                oc = or_satisfy_events(oc, ie.getEvents(), terminalResult);
                //这里，我们认为如果没有下一步的可接受集合，就说明本次推理失败，
                //虽然这个观点对于v_a这种单步表达式是错误的，但是，对于一个能够连续
                //运行的程序，不能看单步的结果，因此使用size（）==0这种方法判断有
                //一定的合理性
                if(oc->ands.size() == 0 || (isEnd == true && terminalResult == false))
                {
                    Solution s;
                    s.type = Solution::FUNCTION;
                    s.functionName = automata->solution;
                    cout << "推理失败，停止" << endl;
                    return s;
                }
                else
                {
                    Solution s;
                    s.type = Solution::CORRECT;
                    cout << "推理成功，进入下一步" << endl;
                    return s;
                }
            }
        public:
            string ruleName;
            automata_type *automata;
            //or_collection *next_oc;
            vector<pair<or_collection *, int> > oc_level;
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
                //查找本自动机中所有的自动机
                for(vector<pair<InterestEvents *, Automata *> >::iterator it = automatas_.begin();
                        it != automatas_.end(); it++)
                {
                    InterestEvents *pIE = (*it).first;
                    Automata *pA = (*it).second;
                    //这里，我们要判断是不是开头事件，或者是结束事件
                    //如果是开头
                    if(pIE->isBegin(newEvent))
                    {
                        cout << "开头事件来了" << endl;
                        //需要在自动机中开启一个新的推理集合
                        pA->newAutomata();
                    }

                    //如果是结尾
                    if(pIE->isEnd(newEvent))
                    {
                        cout << "结尾事件来了" << endl;
                        pA->getSolutionAll(*resultptr_, *pIE, true);
                        pA->deleteAutomata();
                    }

                    //如果包含在推理过程中
                    if(pIE->hasMember(newEvent))
                    {
                        cout << "推理事件来了" << endl;
                        //推理过程
                        pIE->setStatus(newEvent);
                        pA->getSolutionAll(*resultptr_, *pIE, false);

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

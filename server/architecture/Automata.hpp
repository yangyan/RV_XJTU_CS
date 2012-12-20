#ifndef AUTOMATA_CLASS
#define AUTOMATA_CLASS

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

namespace rv_xjtu_yangyan
{
    class Automata /*{{{*/
    {
        public:
            //内部实例类
            class ConcreteAutomata/*{{{*/
            {
                public:
                    ConcreteAutomata(automata_type *at, InterestEvents &ie)/*{{{*/
                        :automataType(at), interestEvents(ie)
                    {
                        //获得推理集合
                        oc = get_or_collection(automataType);
                        //设置当前调用层次
                        level = 0;
                        //清空变量集合
                        BOOST_FOREACH(string &var_name, at->keyvar->vars)
                        {
                            keyTable[var_name] = "";
                        }
                        BOOST_FOREACH(string &var_name, at->nonkeyvar->vars)
                        {
                            nonkeyTable[var_name] = "";
                        }
                        //设置事件状态（如果是函数，表示是否发生）
                        statusInit();

                        boost::uuids::random_generator gen;
                        _uuid = gen();
                    }/*}}}*/

                private:
                    //设置所有事件的状态
                    void statusInit()/*{{{*/
                    {
                        for(vector<InterestEvents::TemplateEvent>::iterator it = interestEvents.events.begin();
                                it != interestEvents.events.end(); it++)
                        {
                            //不考虑函数的多态性，因此函数名可以唯一决定一个事件
                            eventStatus[(*it).eventName] = false;
                        }
                    }/*}}}*/
                    //通过解析event的形式来判断状态
                public:
                    void setSolution(Solution &s)
                    {
                        bool allParaIsReady = true;
                        //检查是否所有参数都齐备
                        BOOST_FOREACH(string &para, automataType->solution->paras)
                        {
                            if((keyTable.find(para) != keyTable.end() 
                                        && keyTable[para] != "")
                                    || (nonkeyTable.find(para) != nonkeyTable.end()
                                        && nonkeyTable[para] != ""))
                            {
                                //能找到一个已经赋值过的参数
                            }
                            else
                            {
                                allParaIsReady = false;
                            }
                        }

                        if(allParaIsReady == true)
                        {
                            s.functionName = automataType->solution->solution_name;
                            BOOST_FOREACH(string &para, automataType->solution->paras)
                            {
                                if(keyTable.find(para) != keyTable.end())
                                {
                                    s.argumentList.append<string>(keyTable[para]);
                                }
                                else
                                {
                                    s.argumentList.append<string>(nonkeyTable[para]);
                                }
                            }
                        }
                        else
                        {
                            cerr << "解决方法参数不齐备，不能设置解决方法" << endl;
                            s.type = Solution::INVALID_FUNCTION;
                        }

                    }

                    bool isKeyEmpty()
                    {
                        for(map<string, string>::iterator it = keyTable.begin();
                                it != keyTable.end(); it++)
                        {
                            if((*it).second != "") return false;
                        }
                        return true;
                    }

                    bool isKeySatisfied(Event &event)
                    {
                        map<string, string> nvpair = interestEvents.getNameValuePair(event);
                        for(map<string, string>::iterator it = keyTable.begin();
                                it != keyTable.end(); it++)
                        {
                            if((*it).second != "") //key表中该项不为空
                            {
                                if(nvpair.find((*it).first) != nvpair.end())//并且事件中也有该项
                                {
                                    if((*it).second != nvpair[(*it).first]) //如果两个值不相同，则不对
                                        return false;
                                }
                            }
                        }
                        return true;
                    }
                    bool isNonKeySatisfied(Event &event)
                    {
                        map<string, string> nvpair = interestEvents.getNameValuePair(event);
                        for(map<string, string>::iterator it = nonkeyTable.begin();
                                it != nonkeyTable.end(); it++)
                        {
                            if((*it).second != "") //key表中该项不为空
                            {
                                if(nvpair.find((*it).first) != nvpair.end())//并且事件中也有该项
                                {
                                    if((*it).second != nvpair[(*it).first]) //如果两个值不相同，则不对
                                        return false;
                                }
                            }
                        }
                        return true;
                    }

                    void setStatus(Event event)/*{{{*/
                    {
                        //设置事件的状态，当前事件为true，其他的都为false，因为已经过去！！！
                        if(interestEvents.hasMember(event))
                        {
                            for(map<string, bool>::iterator it = eventStatus.begin();
                                    it != eventStatus.end(); it++)
                                (*it).second = false;
                            eventStatus[event.eventName] = true;
                        }
                    }/*}}}*/
                    //获取事件，以～e的形式
                    vector<string> getEvents()/*{{{*/
                    {
                        vector<string> rv;
                        for(map<string, bool>::iterator it = eventStatus.begin();
                                it != eventStatus.end(); it++)
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
                    }/*}}}*/
                    //设置变量状态
                    void fillVars(Event &event)/*{{{*/
                    {
                        map<string, string> nvpair = interestEvents.getNameValuePair(event);
                        for(map<string, string>::iterator it = nvpair.begin();
                                it != nvpair.end(); it++)
                        {
                            if(keyTable.find((*it).first) != keyTable.end())
                            {
                                keyTable[(*it).first] = (*it).second;
                            }
                            else if(nonkeyTable.find((*it).first) != nonkeyTable.end())
                            {
                                nonkeyTable[(*it).first] = (*it).second;
                            }
                            else
                            {
                                cerr << "找不到对应的事件" << endl;
                            }
                        }
                    }/*}}}*/

                public:
                    automata_type *automataType;
                    or_collection *oc;
                    unsigned int level;
                    map<string, string> keyTable;
                    map<string, string> nonkeyTable;
                    map<string, bool> eventStatus;
                    InterestEvents interestEvents;

                    boost::uuids::uuid _uuid;
                    bool terminalResult;
                    bool reasonResult;

            };/*}}}*/

            Automata(string rname, automata_type *at)
                :automata(at), ruleName(rname), interestEvents(at)
            {
            }

            void newConcreteAutomata()
            {
                ConcreteAutomata ca(automata, interestEvents);
                conAutos.push_back(ca);
            }

            unsigned int ConcreteAutomataSize()
            {
                return conAutos.size();
            }
            void deleteAutomata()
            {
                for(vector<ConcreteAutomata>::iterator it = conAutos.begin();
                        it != conAutos.end();)//此处不需要++
                {
                    if((*it).level == 0) it = conAutos.erase(it);
                    else it++;
                }
            }

            bool hasEmptyConcreteAutomata()
            {
                BOOST_FOREACH(ConcreteAutomata &ca, conAutos)
                {
                    if(ca.isKeyEmpty())
                        return true;
                }
                return false;
            }

            bool noKeySatisfiedConcreteAutomata(Event &event)
            {
                BOOST_FOREACH(ConcreteAutomata &ca, conAutos)
                {
                    if(ca.isKeySatisfied(event))
                        return false;
                }
                return true;
            }

            //获得自动机中所有解决方案
            void getSolutionAll(Result &rslt, Event &newEvent)
            {
                /*
                 *一个新事件的到达，可能会出现以下情况：
                 *   1、是一个开头事件，规定开头事件要以@begin结尾
                 *   2、是一个普通事件，不以@begin和@end结尾
                 *   3、是一个结束事件，规定结束事件要以@end结尾
                 *   如果是开头事件，那么需要重新开启一个具体自动机
                 *   如果是普通事件，则分以下情况：
                 *      1、已存在开启的具体自动机，并且事件关键参数为空，那么填写好事件参数，推理
                 *      2、已存在开启的具体自动机，并且事件关键参数符合，非关键参数符合，那么继续推理
                 *      3、已存在开启的具体自动机，并且事件关键参数符合，非关键参数不符合，那么规则错误，需要重新填写规则（错误提示)
                 *      4、已存在开启的具体自动机，并且事件关键参数不符合，那么开启一个新的具体自动机
                 *      5、不存在已开启具体自动机，说明之前没有开头事件，那么舍弃这个普通事件
                 *   如果是结尾事件，推理之，然后删除自动机（考虑同层和非同层机制）
                 *
                 */
                bool isEnd;
                //这里，我们要判断是不是开头事件，或者是结束事件
                //如果是开头
                if(interestEvents.isBegin(newEvent))
                {
                    cout << "开头事件来了" << endl;
                    newConcreteAutomata();
                    return;
                }
                //如果是一个普通事件
                if(interestEvents.hasMember(newEvent))
                {/*{{{*/
                    cout << "推理事件来了" << endl;
                    isEnd = false;
                    if(ConcreteAutomataSize() != 0)//存在开启的自动机
                    {
                        if(hasEmptyConcreteAutomata())//有事件关键参数为空
                        {
                            if(ConcreteAutomataSize() == 1)//必须只存在一个这个样的自动机
                            {
                                ConcreteAutomata &ca = conAutos.at(0);
                                ca.fillVars(newEvent);
                                ca.setStatus(newEvent);

                                Solution s = is_satisfy(ca, isEnd);
                                rslt.pushBackSolution(s);
                                return;
                            }
                            else
                            {
                                cerr << "错误：空参数自动机数量不能大于1" << endl;
                                return;
                            }
                        }
                        else                            //所有事件关键参数为非空
                        {
                            if(noKeySatisfiedConcreteAutomata(newEvent))//没有关键字符合的自动机
                            {
                                newConcreteAutomata();
                                ConcreteAutomata &ca = conAutos.back();
                                ca.fillVars(newEvent);
                                ca.setStatus(newEvent);

                                Solution s = is_satisfy(ca, isEnd);
                                rslt.pushBackSolution(s);
                                return; 
                            }
                            else  //有关键字符合的自动机
                            {
                                BOOST_FOREACH(ConcreteAutomata &ca, conAutos)
                                {
                                    if(ca.isKeySatisfied(newEvent) && ca.isNonKeySatisfied(newEvent))//关键和非关键参数都符合
                                    {
                                        ca.fillVars(newEvent);
                                        ca.setStatus(newEvent);

                                        Solution s = is_satisfy(ca, isEnd);
                                        rslt.pushBackSolution(s);
                                    }
                                    else                                //非关键参数不符合
                                    {
                                        //什么都不做
                                    }
                                }
                                return;
                            }

                        }
                    }
                    else
                    {
                        //不存在开启的具体自动机
                        return;
                    }
                }/*}}}*/

                //如果是结尾
                if(interestEvents.isEnd(newEvent))
                {
                    cout << "结尾事件来了" << endl;
                    isEnd = true;

                    BOOST_FOREACH(ConcreteAutomata &ca, conAutos)
                    {
                        Solution s = is_satisfy(ca, isEnd);
                        rslt.pushBackSolution(s);
                    }
                    deleteAutomata();
                    return;
                }
            }
            //检测自动机当前状态是否符合输入的事件，并获取下一步时间的值
            Solution is_satisfy(ConcreteAutomata &ca, bool isEnd)
            {
                or_collection *&oc = ca.oc;
                vector<string> events = ca.getEvents();

                cout << "当前自动机为：" << ca._uuid << endl;
                cout << "当前集合为：";
                or_collection_printer(oc);
                cout << "输入事件为：";
                BOOST_FOREACH(string &e, events)
                {
                    cout << " " << e;
                }

                //如果是结尾事件，那么可以提前结束了
                if(isEnd == true && ca.terminalResult == false)
                {
                    Solution s;
                    s.type = Solution::FUNCTION;
                    ca.setSolution(s);
                    cout << "\n$$$结尾，推理失败" << endl;
                    return s;
                }
                else if(isEnd == true && ca.terminalResult == true)
                {
                    Solution s;
                    s.type = Solution::CORRECT;
                    cout << "\n$$$结尾，推理成功" << endl;
                    return s;
                }

                //对于无法推理下去的集合，后面的解决方案都是ignore
                if(oc->ands.size() == 0)
                {
                    Solution s;
                    s.type = Solution::NO_SOLUTION;
                    cout << "\n无法推理下去了，忽略" << endl;
                    return s;
                }

                vector<string> nowEvents;
                oc = or_satisfy_events(oc, events, ca.reasonResult, ca.terminalResult);

                cout << "\n";
                cout << "下一步集合为：";
                or_collection_printer(oc);

                if(ca.reasonResult == false)
                {
                    Solution s;
                    s.type = Solution::FUNCTION;
                    ca.setSolution(s);
                    cout << "推理失败，停止" << endl;
                    ca.terminalResult = true; //防止结尾的时候再次触发错误
                    return s;
                }
                else if(oc->ands.size() == 0)
                {
                    Solution s;
                    s.type = Solution::CORRECT;
                    cout << "推理成功，但是已经可以终结了！！" << endl;
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
            vector<ConcreteAutomata> conAutos;
            InterestEvents interestEvents;
    };/*}}}*/
}

#endif

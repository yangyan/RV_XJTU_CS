#ifndef SCOPE_CLASS
#define SCOPE_CLASS

#include  <vector>
#include  <iostream>

#include    "Automata.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    class Scope 
    {
    public:
        //内部类
        class ConcreteScope
        {
            //具体的Scope，作用是记录所有的作用域与自动机的对应
        public:
            bool isMatch(string &beg, string &end)
            {
                if(beginEvent == beg && endEvent == end)
                    return true;
                else
                    return false;
            }

            bool isInterestIn(Event &newEvent)
            {
                bool hasInterest = false;
                BOOST_FOREACH(Automata &a, automatas)
                {
                    if(a.interestEvents.hasMember(newEvent)
                            || a.interestEvents.isBegin(newEvent) 
                            || a.interestEvents.isEnd(newEvent))
                        hasInterest = true;
                }
                if(hasInterest == true) 
                    return true;
                else
                    return false;
            }

            bool isInitialized(Event &newEvent)
            {
                bool hasInitialized = false;
                BOOST_FOREACH(Automata &a, automatas)
                {
                    BOOST_FOREACH(Automata::ConcreteAutomata &ca, a.conAutos)
                    {
                        if(ca.isKeyMatch(newEvent))
                            hasInitialized = true;
                    }
                }
                if(hasInitialized == true)
                    return true;
                else
                    return false;
            }

            void getSolutionAll(Result &result, Event &newEvent)
            {
                BOOST_FOREACH(Automata &a, automatas)
                {
                    a.getSolutionAll(result, newEvent);
                }
            }

        public:
            string beginEvent;
            string endEvent;
            vector<Automata> automatas;
        };

    public:
        //函数
        void appendScopeAutomatas(string rule_name, automata_type *at)
        {
            bool matched = false;
            string beginEvent = LEAF_P(at->scope->begin)->event_name + "@begin";
            string endEvent = LEAF_P(at->scope->end)->event_name + "@end";
            BOOST_FOREACH(ConcreteScope &cs, allScopes)
            {
                //其实是取决与beginEvent的，因为默认beginEvent和endEvent应该是一样的
                if(cs.isMatch(beginEvent, endEvent))
                {
                    Automata a = Automata(rule_name, at);
                    cs.automatas.push_back(a);
                    matched = true;
                }
            }
            if(matched == false)
            {
                ConcreteScope cs;
                cs.beginEvent = beginEvent;
                cs.endEvent = endEvent;
                Automata a = Automata(rule_name, at);
                cs.automatas.push_back(a);
                allScopes.push_back(cs);
            }
        }

        void getSolutionRecursion(vector<ConcreteScope>::reverse_iterator itThisCS, Result  &result, Event &newEvent)
        {
            //如果已经结束了
            if(itThisCS == scopeSequence.rend())
            {
                cerr << "没有找到合适的推理Scope" << endl;
                return;
            }

            //1.  若本层关注e，且上层所有都不关注e，那么本层处理。
            //2.  若本层关注e，且上层有关注e的，但都没有实例化，那么本层处理。
            //3.  若本层关注e，且上层有关注e的，也有上层有实例化的，那么交给上层实例化的处理。
            //4.  若本层不关注e，交给直接上层处理。

            ConcreteScope &current = (*itThisCS);
            if(current.isInterestIn(newEvent))
            {
                bool hasInterestScope = false;
                vector<ConcreteScope>::reverse_iterator it = scopeSequence.rbegin();
                it++;
                for(; it != scopeSequence.rend(); it++)
                {
                    if((*it).isInterestIn(newEvent))
                        hasInterestScope = true;
                }

                if(hasInterestScope == false)
                {
                    //情况1
                    cout << "情况1" << endl;
                    (*itThisCS).getSolutionAll(result, newEvent);
                }
                else
                {
                    bool hasInitializedScope = false;
                    ConcreteScope *thatCS;
                    vector<ConcreteScope>::reverse_iterator it = scopeSequence.rbegin();
                    it++;
                    for(; it != scopeSequence.rend(); it++)
                    {
                        if((*it).isInitialized(newEvent))
                        {
                            thatCS = &(*it);
                            hasInitializedScope = true;
                            break;
                        }
                    }
                    if(hasInitializedScope == false)
                    {
                        //情况2
                        cout << "情况2" << endl;
                        (*itThisCS).getSolutionAll(result, newEvent);
                    }
                    else
                    {
                        //情况3
                        cout << "情况3" << endl;
                        thatCS->getSolutionAll(result, newEvent);
                    }

                }

            }
            else
            {
                //情况4
                cout << "情况4" << endl;
                itThisCS++;
                getSolutionRecursion(itThisCS, result, newEvent);
            }
        }
        void getSolutionAll(Result &result, Event &newEvent)
        {
            ////////////////////////////////////
            //打印当前层次
            cout << "当前层次   ";
            BOOST_FOREACH(ConcreteScope &cs, scopeSequence)
            {
                cout << cs.beginEvent << " >> ";
            }
            cout << endl;
            ////////////////////////////////////
            //Scope和Scope之间的关注事件应该没有交集，但Scope中的不同自动机可以关注同一个事件
            vector<ConcreteScope>::reverse_iterator it = scopeSequence.rbegin();
            getSolutionRecursion(it, result, newEvent);
        }

        void buildScopeLevelAndGetSolutionAll(Result &result, Event &newEvent)
        {
            if(isBeginEvent(newEvent))
            {
                //需要建立层次
                //这里是假设begin和end是同一事件的，因此，可以形成配对
                BOOST_FOREACH(ConcreteScope &cs, allScopes)
                {
                    if(newEvent.eventName == cs.beginEvent)
                    {
                        scopeSequence.push_back(cs);
                    }
                }
                getSolutionAll(result, newEvent);
            }
            else if(isEndEvent(newEvent))
            {
                //需要销毁层次
                getSolutionAll(result, newEvent);
                if(scopeSequence.size() > 0 && scopeSequence.back().endEvent == newEvent.eventName)
                {
                    scopeSequence.pop_back();
                }
                else
                {
                    cerr << "不匹配的Scope，可以忽略" << endl;
                }
            }
            else
            {
                if(scopeSequence.size() > 0)
                {
                    getSolutionAll(result, newEvent);
                }
                else
                {
                    cerr << "这个事件不在一个Scope里，忽略" << endl;
                }
            }
        }
    private:
        bool isBeginEvent(Event &e)
        {
           if(e.eventName.find("@begin") != string::npos)
              return true; 
        }
        bool isEndEvent(Event &e)
        {
           if(e.eventName.find("@end") != string::npos)
              return true; 
        }

    public:
        //成员变量
        //记录所有的Scope(在配置文件中拥有的)
        vector<ConcreteScope> allScopes;
        //记录运行过程中遇到的Scope中的begin和end
        vector<ConcreteScope> scopeSequence;
    };
}

#endif

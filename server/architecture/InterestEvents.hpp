#ifndef INTERESTEVENTS_HPP
#define INTERESTEVENTS_HPP

#include  <vector>
#include  <iostream>

#include    "../../modules/event/Event.hpp"
#include    "../../modules/solution/Result.hpp"
#include    "../../modules/basic/semaphore.hpp"
#include    "../../modules/solution/Solution.hpp"

#include    "../automata/automata.hpp"
#include    "../automata/reasonning.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;
    class InterestEvents/*{{{*/
    {
        public:
            class TemplateEvent/*{{{*/
            {
                public:
                    TemplateEvent(string &name)/*{{{*/
                    {
                        eventName = name;
                    }/*}}}*/

                    void addPara(string &p)/*{{{*/
                    {
                        paras.push_back(p);
                    }
                    /*}}}*/
                public:
                    string eventName;
                    vector<string> paras;
            };/*}}}*/

            InterestEvents(automata_type *at)/*{{{*/
            {
                fillEvents(events, at);
            }/*}}}*/

            //检查是否包含某个事件
            bool hasMember(const Event &event)/*{{{*/
            {
                for(vector<TemplateEvent>::iterator it = events.begin();
                        it != events.end(); it++)
                {
                    if(event.eventName == (*it).eventName 
                            || (event.eventName.at(0) == '~' && event.eventName.substr(1) == (*it).eventName))
                    {
                        return true;
                    }
                }
                return false;
            }/*}}}*/

            //检查是不是开头事件
            bool isBegin(const Event &event)/*{{{*/
            {
                if(event.eventName == beginEvent)
                    return true;
                else 
                    return false;
            }/*}}}*/
            //检查是不是结尾事件
            bool isEnd(const Event &event)/*{{{*/
            {
                if(event.eventName == endEvent)
                    return true;
                else
                    return false;
            }/*}}}*/

            /*
             *            void print()[>{{{<]
             *            {
             *                for(map<string, bool>::iterator it = status_.begin();
             *                        it != status_.end(); it++)
             *                {
             *                    if((*it).second == true)
             *                    {
             *                        cout << ((*it).first) << " ";
             *                    }
             *                    else
             *                    {
             *                        cout << ("~" + (*it).first) << " ";
             *                    }
             *                }
             *                cout << endl;
             *            }
             *[>}}}<]
             */

            TemplateEvent find(string eventName)/*{{{*/
            {
                BOOST_FOREACH(TemplateEvent &te, events)
                {
                    if(te.eventName == eventName)
                        return te;
                }
                string emptyString;
                return TemplateEvent(emptyString);
            }/*}}}*/

            map<string, string> getNameValuePair(Event &event)/*{{{*/
            {
                map<string, string> rv;
                TemplateEvent te = find(event.eventName);
                if(te.eventName == "") return rv;//空的
                else
                {
                    int index = 0;
                    BOOST_FOREACH(string &para, te.paras)
                    {
                        rv[para] = event.functionArgs[index++].toString();
                        if(rv[para] == "")
                            cerr << "事件参数设置数量不对" << endl;
                    }
                    return rv;
                }
            }/*}}}*/


        private:
            ////////////////////////////////////////////////////////////////////
            //获取一个自动机器中所有的事件
            ////////////////////////////////////////////////////////////////////
            void get_events_from_at(vector<TemplateEvent> &events, automata_type *at)/*{{{*/
            {
                if(at->type == "leaf" && LEAF_P(at)->is_true_leaf == false)
                {
                    TemplateEvent te(LEAF_P(at)->event_name);
                    BOOST_FOREACH(string &para, LEAF_P(at)->paras)
                    {
                        te.addPara(para);
                    }
                    events.push_back(te);
                }
                else if(at->type == "node")
                {
                    get_events_from_at(events, NODE_P(at)->left_automata);
                    get_events_from_at(events, NODE_P(at)->right_automata);
                    if(NODE_P(at)->extra_automata != NULL)
                        get_events_from_at(events, NODE_P(at)->extra_automata);
                }
            }/*}}}*/

            //从自动机中获取所有事件，不包含事件的状态
            void fillEvents(vector<TemplateEvent> &es, automata_type *at)/*{{{*/
            {
                //从自动机公式中获得事件
                get_events_from_at(es, at);
                //从scope中获得事件，这个事件为函数事件也就是f_xxxx
                if(at->scope->samelevel == true)
                {
                    //对于within中的事件，需要添加额外的后缀来区别开头和结尾
                    beginEvent = (at->scope->begin->event_name) + "@begin";
                    endEvent = (at->scope->end->event_name) + "@end";
                }
                else
                {
                    beginEvent = (at->scope->begin->event_name) + "@begin";
                    endEvent = (at->scope->end->event_name) + "@end";
                }
            }/*}}}*/

        public:
            vector<TemplateEvent> events;
            string beginEvent;
            string endEvent;
    };/*}}}*/
}
#endif

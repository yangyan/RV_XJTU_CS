#ifndef REASONNING_HPP
#define REASONNING_HPP

#include  <set>
#include  <algorithm>
#include  <iostream>

#include    "automata.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    struct event_comparer
    {
        bool operator()(automata_leaf *lh, automata_leaf *rh)
        {
            if(lh->is_negative != rh->is_negative)
            {
                if(lh->is_negative == false)
                    return false;
                else
                    return true;
            }
            else
            {
                if(lh->event_name != rh->event_name)
                {
                    return lh->event_name < rh->event_name;
                }
                else
                {
                    return lh->next_node < rh->next_node;
                }
            }
        }
    };

    struct and_collection
    {
        and_collection(){}
        set<automata_leaf *, event_comparer> events;
    };

    struct and_comparer
    {
        bool operator()(and_collection *lh, and_collection *rh)
        {
            if(lh->events.size() == rh->events.size())
            {
                return lh->events < rh->events;
            }
            else
            {
                return lh->events.size() < rh->events.size();
            }
        }
    };

    struct or_collection
    {
        or_collection(){}
        //这里，没有定义set的比较函数，如果定义，可以实现化简的功能
        set<and_collection *, and_comparer> ands;
    };

    or_collection *get_or_collection(automata_type *at)
    {
        if(at->type == "leaf")
        {
            and_collection *andp = new and_collection();
            andp->events.insert(LEAF_P(at));
            or_collection *orp = new or_collection();
            orp->ands.insert(andp);
            return orp;
        }
        else if(at->type == "node")
        {
            if(NODE_P(at)->relationship == "or")
            {
                or_collection *orp1 = get_or_collection(NODE_P(at)->left_automata);
                or_collection *orp2 = get_or_collection(NODE_P(at)->right_automata);
                or_collection *orp = new or_collection();
                vector<and_collection *> andv;
                
                set_union(orp1->ands.begin(), orp1->ands.end(),
                        orp2->ands.begin(), orp2->ands.end(), back_inserter(andv));
                orp->ands = set<and_collection *, and_comparer>(andv.begin(), andv.end());
                /*
                 *delete orp1;
                 *delete orp2;
                 */
                return orp;
            }
            else if(NODE_P(at)->relationship == "and")
            {
                or_collection *orp1 = get_or_collection(NODE_P(at)->left_automata);
                or_collection *orp2 = get_or_collection(NODE_P(at)->right_automata);
                or_collection *orp = new or_collection();
                for(set<and_collection *, and_comparer>::iterator it1 = orp1->ands.begin();
                        it1 != orp1->ands.end(); it1++)
                {
                    for(set<and_collection *, and_comparer>::iterator it2 = orp2->ands.begin();
                            it2 != orp2->ands.end(); it2++)
                    {
                        and_collection *and_tmp = new and_collection();
                        vector<automata_leaf *> eventv;
                        set_union((*it1)->events.begin(), (*it1)->events.end(),
                                (*it2)->events.begin(), (*it2)->events.end(), back_inserter(eventv));
                        and_tmp->events = set<automata_leaf *, event_comparer>(eventv.begin(), eventv.end());
                        orp->ands.insert(and_tmp);
                    }
                }
                return orp;
            }
            cerr << "出错了，推理过程中遇到了不可知的结点1" << endl;
            return NULL;
        }
        cerr << "出错了，推理过程中遇到了不可知的结点2" << endl;
        return NULL;
    };

    void or_collection_printer(or_collection *orp)
    {
        for(set<and_collection *, and_comparer>::iterator and_it = orp->ands.begin();
                and_it != orp->ands.end(); and_it++)
        {
            cout << (and_it == orp->ands.begin() ? "":"|");
            cout << "(";
            for(set<automata_leaf *, event_comparer>::iterator event_iter = (*and_it)->events.begin();
                    event_iter != (*and_it)->events.end();  event_iter++)
            {
                cout << (event_iter == (*and_it)->events.begin() ? "":"& ");
                cout << ((*event_iter)->is_negative?"~":"") << (*event_iter)->event_name  << " ";
            }
            cout << ")" ;
        }
        cout << endl;
    }

    bool and_satisfy_events(and_collection *ac, vector<string> &events, bool &andTerminalResult)
    {
        andTerminalResult = true;
        for(set<automata_leaf *, event_comparer>::iterator it = ac->events.begin();
                it != ac->events.end(); it++)
        {
            if((*it)->is_true_leaf == true && (*it)->is_acceptable == false)
            {
                andTerminalResult = false;
                break;
            }
        }

        //判断一个事件是否符合，只需要查看其相反事件是否存在
        for(vector<string>::iterator it = events.begin();
                it != events.end(); it++)
        {
            if((*it).at(0) != '~')
            {
                //对于正时间，查找其相反事件
                string event = (*it);
                for(set<automata_leaf *, event_comparer>::iterator sit = ac->events.begin();
                        sit != ac->events.end(); sit++)
                {
                    if((*sit)->is_negative == true 
                            && (*sit)->event_name == event)
                    {
                        andTerminalResult = false;
                        return false;
                    }
                }
            }
            else
            {
                string event = (*it).substr(1);
                for(set<automata_leaf *, event_comparer>::iterator sit = ac->events.begin();
                        sit != ac->events.end(); sit++)
                {
                    if((*sit)->is_negative == false
                            && (*sit)->event_name == event)
                    {
                        andTerminalResult = false;
                        return false;
                    }
                }
            }
        }
        return true;
    }

    or_collection *get_next(and_collection *ac)
    {
        or_collection *rv_oc = new or_collection();
        for(set<automata_leaf *, event_comparer>::iterator it = ac->events.begin();
                it != ac->events.end(); it++)
        {
            if((*it)->event_name == "true")
            {
                or_collection *tmp_oc;
                tmp_oc = get_or_collection((*it)->next_node);
                for(set<and_collection *, and_comparer>::iterator sit = tmp_oc->ands.begin();
                        sit != tmp_oc->ands.end(); sit++)
                {
                    rv_oc->ands.insert(*sit);
                }
            }
        }
        return rv_oc;
    }
                
    or_collection *or_satisfy_events(or_collection *lastest, vector<string> events, bool &terminalResult)
    {
        terminalResult = false;
        or_collection *rv_oc = new or_collection();
        for(set<and_collection *, and_comparer>::iterator it = lastest->ands.begin();
                it != lastest->ands.end(); it++)
        {
            bool andTerminalResult;
            if(and_satisfy_events(*it, events, andTerminalResult))
            {
                or_collection *tmp_oc = get_next(*it);
                for(set<and_collection *, and_comparer>::iterator sit = tmp_oc->ands.begin();
                        sit != tmp_oc->ands.end(); sit++)
                {
                    rv_oc->ands.insert(*sit);
                }
            }
            else
            {
                //什么都不做
            }
            terminalResult = terminalResult || andTerminalResult;
        }
        return rv_oc;
    }

}

#endif

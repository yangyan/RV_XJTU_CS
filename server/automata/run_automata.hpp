#ifndef RUN_AUTOMATA_HPP
#define RUN_AUTOMATA_HPP

#include  <queue>
#include  <iostream>
#include  <vector>

#include    "automata.hpp"

namespace rv_xjtu_yangyan
{
#define LEAF_P(p) ((automata_leaf *)p)
#define NODE_P(p) ((automata_node *)p)

    bool is_satisfy(automata_type *at, std::vector<std::string> &events)
    {
        std::cout << at->type << std::endl;
        if(at->type == "leaf")
        {
            //是一个叶子结点
            if(LEAF_P(at)->is_true_leaf == true)
            {
                return true;
            }
            else if(LEAF_P(at)->is_negative == false)
            {
                std::vector<std::string>::iterator rst;
                rst = std::find(events.begin(), events.end(), LEAF_P(at)->event_name);
                if(rst == events.end())
                {
                    //没有找到
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else if(LEAF_P(at)->is_negative == true)
            {
                std::vector<std::string>::iterator rst;
                rst = std::find(events.begin(), events.end(), "~" + LEAF_P(at)->event_name);
                if(rst == events.end())
                {
                    //没有找到
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
        else
        {
            //这不是一个叶子结点
            bool lh_result = is_satisfy(NODE_P(at)->left_automata, events);
            bool rh_result = is_satisfy(NODE_P(at)->right_automata, events);
            if(NODE_P(at)->relationship == "and")
            {
                return lh_result && rh_result;
            }
            else if(NODE_P(at)->relationship == "or")
            {
                return lh_result || rh_result;
            }
        }
        std::cerr << "奇怪，没有符合的结点类型" << std::endl;
        return false;
    }

    struct run_automata_collection
    {
        void operator()(automata_collection *ac)
        {
            for(std::vector<automata_type *>::iterator it =  ac->automatas.begin();
                    it != ac->automatas.end(); it++)
            {
                std::vector<std::string> events;
                std::string s;
                std::cin >> s;
                events.push_back(s);
                std::cin >> s;
                events.push_back(s);
                std::cin >> s;
                events.push_back(s);
                if(is_satisfy(*it, events))
                {
                    std::cout << it - ac->automatas.begin() <<":符合" << std::endl;
                }
                else
                {
                    std::cout << it - ac->automatas.begin() <<":不符合" << std::endl;
                }
            }
        }
    };
}

#endif

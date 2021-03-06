#ifndef RUN_AUTOMATA_HPP
#define RUN_AUTOMATA_HPP

#include  <queue>
#include  <iostream>
#include  <vector>

#include    "automata.hpp"
#include    "reasonning.hpp"

namespace rv_xjtu_yangyan
{
    struct run_automata_rule
    {
        void operator()(automata_rule *ar)
        {
            for(std::vector<automata_type *>::iterator it =  ar->automatas.begin();
                    it != ar->automatas.end(); it++)
            {
                std::vector<std::string> events;
                or_collection *next;
                or_collection_printer(next = get_or_collection(*it));
                std::cout << std::endl;

                //以下代码是测试是否符合事件的
                while(true)
                {
                    std::string s1, s2 , s3;
                    std::cin >> s1 >> s2 >> s3;
                    events.push_back(s1);
                    events.push_back(s2);
                    events.push_back(s3);
                    bool b;
                    or_collection_printer(next = or_satisfy_events(next, events, b));
                    std::cout << std::endl;
                    events.clear();
                    sleep(1);
                }

            }
        }
    };
}

#endif

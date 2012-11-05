#ifndef RUN_AUTOMATA_HPP
#define RUN_AUTOMATA_HPP

#include  <queue>
#include  <iostream>
#include  <vector>

#include    "automata.hpp"
#include    "reasonning.hpp"

namespace rv_xjtu_yangyan
{
    struct run_automata_collection
    {
        void operator()(automata_collection *ac)
        {
            for(std::vector<automata_type *>::iterator it =  ac->automatas.begin();
                    it != ac->automatas.end(); it++)
            {
                std::vector<std::string> events;
                or_collection *next;
                or_collection_printer(next = get_or_collection(*it));
                std::cout << "~~~~~~~~~~~~~~~~~~~~"  << std::endl;
                while(true)
                {
                std::string s1, s2 , s3;
                std::cin >> s1 >> s2 >> s3;
                events.push_back(s1);
                events.push_back(s2);
                events.push_back(s3);
                or_collection_printer(next = or_satisfy_events(next, events));
                std::cout << "~~~~~~~~~~~~~~~~~~~~"  << std::endl;
                events.clear();
                sleep(1);
                }

            }
        }
    };
}

#endif

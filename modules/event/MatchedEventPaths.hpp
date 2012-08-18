#ifndef MATCHEDEVENTPATHS_CLASS
#define MATCHEDEVENTPATHS_CLASS

#include  <list>
#include  <algorithm>

#include    "EventPath.hpp"
#include    "EventPathHistory.hpp"
#include    "../solution/Solution.hpp"

namespace rv_xjtu_yangyan {
    //这个类用于记录符合当前事件的路径
    //类主要由下面几个要素构成：
    //1、使用的历史路径集EventPathHistory
    //2、匹配路径的指针
    //3、与2中指针相对应的游标，即当前输入的事件匹配到哪
    //   个位置了。
    //
    //在这个类中，应该考虑多线程的情况
    //需要考察历史路径是什么类别，是线程的？多线程的？
    //这个应该由诊断端来判断

    class MatchedEventPaths
    {
        public:
            typedef struct pp 
            {
                EventPath   *path;
                size_t      position; 
            }pp_t;

            struct GetMatchedHead
            {
                std::list<pp_t *> *dbp_;
                Event *e_;
                GetMatchedHead(std::list<pp_t *> &_db, Event &_e):dbp_(&_db),e_(&_e){}
                void operator()(std::pair<EventPath *, Solution *> _p)
                {
                    std::cout << "first" << std::endl;
                    EventPath &ep = *(_p.first);
                    if(ep[0]->functionName == e_->functionName)
                    {
                        pp_t *pp_tp = new pp_t;
                        pp_tp->path = &ep;
                        pp_tp->position = 0;
                        dbp_->insert(dbp_->begin(), pp_tp);
                    }
                }
            };

            struct GetStillMatched
            {
                std::list<pp_t *> *dbp_;
                Event *e_;
                GetStillMatched(std::list<pp_t *> &_db, Event &_e):dbp_(&_db),e_(&_e){}
                bool operator()(pp_t *_pp)
                {
                    std::cout << "other" << std::endl;
                    EventPath *ep = _pp->path;
                    Event *e = (*ep)[++(_pp->position)];
                    if(ep != NULL && e->functionName == e_->functionName)
                    { return false; } else { return true; }
                }
            };







        public:
            MatchedEventPaths(EventPathHistory &);
            ~MatchedEventPaths();
            void doMatching(Event &);


        private:
            std::list<pp_t *> database_;
            EventPathHistory *history_;
    };

    MatchedEventPaths::MatchedEventPaths(EventPathHistory &_eph):history_(&_eph)
    {
    }

    MatchedEventPaths::~MatchedEventPaths()
    {
    }

    void MatchedEventPaths::doMatching(Event &_e)
    {
        //匹配分为两个部分
        //1、在正在匹配的路径中查找，如果下一个不符合，则要从已匹配队列中删除
        //2、在历史路径中查找,此时position应该为0
        //两者顺序不可以颠倒
        database_.remove_if(GetStillMatched(database_, _e));
        for_each(history_->eventPaths.begin(), history_->eventPaths.end(), GetMatchedHead(database_, _e));
        std::cout << "yes" << std::endl;
    }
}


#endif 

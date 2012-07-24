#include  <iostream>
#include  <fstream>

#include    "EventQueue.hpp"
#include    "EventPath.hpp"
#include    "EventPathHistory.hpp"
#include    "EventTime.hpp"
#include    "EventManager.hpp"

#include  <boost/archive/text_oarchive.hpp>
#include  <boost/archive/text_iarchive.hpp>

using namespace rv_xjtu_yangyan;

EventManager *em = new EventManager();

void genEvent()
{
    Event e = Event();
    em->syncAccept(e);
}

void genEvent2()
{
    Event e = Event();
    em->syncAccept(e);
    Event e2 = Event();
    em->syncAccept(e2);
}

int main(int argc, char *argv[])
{
    //Event e = Event();
    //ArgumentList al = ArgumentList();
    //al.append("abc");
    //al.append("cd");
    //al.append("123");
    //e.setFunctionArgs(al);

    //Event e1 = Event();
    //ArgumentList al2 = ArgumentList();
    //al2.append("ABC");
    //al2.append("DD");
    //al2.append("~~~");
    //e1.setFunctionArgs(al2);

    /*
     *EventQueue eq = EventQueue();
     *eq.append(e);
     *eq.append(e1);
     */

    //e.setFunctionName("a1");
    //e1.setFunctionName("a2");
    //EventPath ep = EventPath();
    //ep.append(e);
    //ep.append(e1);

    //EventPath ep2 = EventPath();
    //ep2.append(e1);
    //ep2.append(e);

    //EventPathHistory eph = EventPathHistory();
    //Solution s = Solution();
    //eph.append(ep,s);
    //eph.append(ep2,s);
    //eph.append(ep2,s);
    //eph.append(ep2,s);

    //eph.getSolution(ep2);

    //std::cout << eph.size() << std::endl;

    //eph.storeToFile("yangyan");

    //EventPathHistory eph2 = EventPathHistory();
    //eph2.readFromFile("yangyan");
    //std::cout << eph2.toString() << std::endl;


/*
 *    std::ofstream ofs("storefile");
 *
 *    boost::archive::text_oarchive oa(ofs);
 *    oa << ep;
 */

/*
 *    std::ifstream ifs("storefile");
 *    boost::archive::text_iarchive ia(ifs);
 *    EventPath ep2 = EventPath();
 *    ia >> ep2;
 *    std::cout << ep2.toString() << std::endl;
 *
 *
 */
   //return 0;

/*
 *    Event e = Event();
 *    Event e2 = Event();
 *
 *    std::cout << e2.timeSince(e) << std::endl;
 */
    em->run();
    boost::thread_group tg;
    tg.create_thread(&genEvent);
    tg.create_thread(&genEvent);
    tg.create_thread(&genEvent);
    tg.create_thread(&genEvent2);
    tg.create_thread(&genEvent2);
    tg.join_all();
    em->safeStop();

    return 0;
}

#ifndef ACTION_CLASS
#define ACTION_CLASS

#include  <vector>
#include  <iostream>
#include  <algorithm>

#include    "../automata/automata.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    class Action
    {
    public:
        Action(automata_type *at)
            :automataType(at),
            isNothing(false),
            isStop(false),
            isIgnore(false),
            isReplace(false)
        {
            isNothing = false;
            //这里设置些动作，用于推理失败时，自动机应该采取的措施

            //无，表示可以自然推理结束
            actionNames.push_back("nothing");
            //停止，表示推理不需要进行下去，一般用于一次触发，非持续性的属性
            actionNames.push_back("stop");
            //忽略，表示省略这一步，用于安全属性中 非事件 的违背
            actionNames.push_back("ignore");
            //代替，表示用某一事件代替当前违反的事件
            actionNames.push_back("replace");

            vector<string>::iterator it;
            string action_name = automataType->action->action_name;
            it = find(actionNames.begin(), actionNames.end(), action_name);
            if(it == actionNames.end())
            {
                cerr << "没有找到对应的Action方法" << endl;
            }
            else
            {
                if(action_name == "nothing") isNothing == true;
                else if(action_name == "stop") isStop = true;
                else if(action_name == "ignore") isIgnore = true;
                else if(action_name == "replace") isReplace = true;
            }
        }

        //对于replace事件，需要从action_name和paras得到一个事件
        void getEventName(bool &positive, string &eventName)
        {
            if(automataType->action->action_name == "replace")
            {
                //第一参数是指定事件的正反，第二个参数是指定事件的名称
                if(automataType->action->paras.size() == 2)
                {
                    if(automataType->action->paras.at(0) == "true")
                    {
                        positive = true;
                    }
                    else if(automataType->action->paras.at(0) == "false")
                    {
                        positive = false;
                    }
                }
                eventName = automataType->action->paras.at(1);
            }
            return;
        }

    public:
        vector<string> actionNames;
        automata_type *automataType;
        bool isNothing;
        bool isStop;
        bool isIgnore;
        bool isReplace;

    };
}

#endif

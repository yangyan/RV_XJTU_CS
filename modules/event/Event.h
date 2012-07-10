#include  <string>
#include  <iostream>

#include  <unistd.h>

#include    "ArgumentList.h"

namespace rv_xjtu_yangyan {
   
   class Event
   {
       /*
        *从进程中获取的程序的信息
        */
       public:
           size_t       processId;
           std::string  processName;
           std::string  functionName;
           ArgumentList functionArgs;

       public:
/*
 *           Event();
 *
 *           ~Event();
 */
           
           void setCurrentPID();
           void setCurrentProcessName(std::string path);
           void setFunctionName(std::string funcName);
           void setFunctionArgs(ArgumentList &funcArgs);
           void clone(Event &event);

   };

}

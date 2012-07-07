using namespace rv_xjtu_yangyan;

namespace rv_xjtu_yangyan {
   
   class Event
   {
       /*
        *data from monitored process
        */
       public:
           unsigned int processId;
           string       functionName;
           ArgumentList functionArgs;
       /*
        *functions of class Event 
        */
       public:
           Event(){
           }

           ~Event(){
           }

   }

}

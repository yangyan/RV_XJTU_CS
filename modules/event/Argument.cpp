#include  <sstream>
#include  <string>

#include    "Argument.h"

namespace rv_xjtu_yangyan {
   
   /*
    *从各种类型的参数构造
    */
   Argument::Argument(std::string arg)
   {
       argumentString = std::string(arg);
   }

   Argument::Argument(int arg)
   {
       std::stringstream ss;
       ss << arg;
       argumentString = ss.str();
   }

   Argument::Argument(double arg)
   {
       std::stringstream ss;
       ss << arg;
       argumentString = ss.str();
   }

   /*
    *字符串显示
    */
   const std::string& Argument::toString() const
   {
       return argumentString;
   }

}

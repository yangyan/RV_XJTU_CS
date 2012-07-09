#ifndef ARGUMENT_CLASS
#define ARGUMENT_CLASS

#include  <sstream>
#include  <string>

namespace rv_xjtu_yangyan {
   
   class Argument
   {
       public:
           //从各种类型的参数构造
           Argument(std::string arg);
           Argument(int arg);
           Argument(double arg);

           //字符串显示
           const std::string& toString() const;

       private:
           //使用string表达参数，所有类型的参数，使用string来表示
           //转化的方法，根据LTL公式中参数的表达方式而定
           std::string argumentString;

   };

}

#endif 

#ifndef ARGUMENT_CLASS
#define ARGUMENT_CLASS

#include  <sstream>
#include  <string>
#include  <cstdlib>

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/string.hpp>

namespace rv_xjtu_yangyan {
   
   class Argument
   {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & _argumentString;
               }

       public:
           //从各种类型的参数构造
           Argument();
           Argument(std::string arg);
           Argument(int arg);
           Argument(double arg);

           //字符串显示 ...
           const std::string toString() const;

           const int toInt() const;
           const double toDouble() const;

       private:
           //使用string表达参数，所有类型的参数，使用string来表示
           //转化的方法，根据LTL公式中参数的表达方式而定
           std::string _argumentString;

   };

}

namespace rv_xjtu_yangyan {
   
   /*
    *从各种类型的参数构造
    */
   Argument::Argument()
   {
   }

   Argument::Argument(std::string arg)
   {
       _argumentString = std::string(arg);
   }

   Argument::Argument(int arg)
   {
       std::stringstream ss;
       ss << arg;
       _argumentString = ss.str();
   }

   Argument::Argument(double arg)
   {
       std::stringstream ss;
       ss << arg;
       _argumentString = ss.str();
   }

   /*
    *字符串显示
    */
   const std::string Argument::toString() const
   {
       return _argumentString;
   }

   const int Argument::toInt() const
   {
       return atoi(_argumentString.c_str());
   }

   const double Argument::toDouble() const
   {
       return atol(_argumentString.c_str());
   }
}

#endif 

#ifndef ARGUMENTLIST_CLASS
#define ARGUMENTLIST_CLASS

#include  <vector>
#include  <sstream>
#include  <iostream>
#include    "Argument.h"

namespace rv_xjtu_yangyan {
   
   class ArgumentList
   {
       public:
           std::vector<Argument *> argumentList;

       public:
           ArgumentList();
           ~ArgumentList();

           //转化为字符串
           const std::string toString() const;
           //下标操作
           const Argument& operator[](const size_t index) const;

           //复制本类
           void clone(const ArgumentList &argList);

           //增加一个参数
           template <typename T>
               void append(const T arg)
               {
                   Argument *argument = new Argument(arg);
                   argumentList.push_back(argument);
               }
   };

}

#endif

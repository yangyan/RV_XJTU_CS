#ifndef ARGUMENTLIST_CLASS
#define ARGUMENTLIST_CLASS

#include  <vector>
#include  <sstream>
#include  <iostream>

#include    "Argument.hpp"

#include  <boost/serialization/access.hpp>
#include  <boost/serialization/vector.hpp>

namespace rv_xjtu_yangyan {
   
   class ArgumentList
   {
       private:
           //串行化
           friend class boost::serialization::access;
           template<class Archive>
               void serialize(Archive &ar, const unsigned int version)
               {
                   ar & argumentList;
               }


       public:
           ArgumentList();

           //转化为字符串 ...
           const std::string toString() const;
           //下标操作
           const Argument operator[](const size_t index) const;
           //返回参数数量
           const size_t size() const;

           //复制本类
           void clone(const ArgumentList &argList);

           //增加一个参数
           template <typename T>
               void append(const T arg)
               {
                   Argument argument(arg);
                   argumentList.push_back(argument);
               }
       public:
           std::vector<Argument> argumentList;
   };

}

namespace rv_xjtu_yangyan {
   
   ArgumentList::ArgumentList()
   {
   }

   /*
    *转化为字符串
    */
   const std::string ArgumentList::toString() const
   {
       std::stringstream ss;
       for(std::vector<Argument>::const_iterator iter = argumentList.begin();
               iter != argumentList.end(); ++iter)
       {
           ss <<  (*iter).toString() << " ";
       }
       return ss.str();
   }
   /*
    *下标操作
    */
   const Argument ArgumentList::operator[](const size_t index) const
   {
       Argument a;
       if(index >= argumentList.size()) return a;
       else return (argumentList[index]);
   }
   /*
    *数量
    */
   const size_t ArgumentList::size() const
   {
       return argumentList.size();
   }

   /*
    *复制类
    */
   void ArgumentList::clone(const ArgumentList &argList)
   {
       for(std::vector<Argument>::const_iterator iter = argList.argumentList.begin();
               iter != argList.argumentList.end(); ++iter)
       {
           this->append((*iter).toString());
       }
   }
}
#endif

#include    "ArgumentList.h"

namespace rv_xjtu_yangyan {
   
   ArgumentList::ArgumentList()
   {
   }

   ArgumentList::~ArgumentList()
   {
       for(std::vector<Argument *>::iterator iter = argumentList.begin();
               iter != argumentList.end(); ++iter)
       {
           delete (*iter);
       }
   }

   /*
    *转化为字符串
    */
   const std::string ArgumentList::toString() const
   {
       std::stringstream ss;
       for(std::vector<Argument *>::const_iterator iter = argumentList.begin();
               iter != argumentList.end(); ++iter)
       {
           std::cout <<  (*iter)->toString() << " ";
       }
       return ss.str();
   }
   /*
    *下标操作
    */
   const Argument& ArgumentList::operator[](const size_t index) const
   {
       return *(argumentList[index]);
   }

   /*
    *复制类
    */
   void ArgumentList::clone(const ArgumentList &argList)
   {
       for(std::vector<Argument *>::const_iterator iter = argList.argumentList.begin();
               iter != argList.argumentList.end(); ++iter)
       {
           this->append((*iter)->toString());
       }
   }
}

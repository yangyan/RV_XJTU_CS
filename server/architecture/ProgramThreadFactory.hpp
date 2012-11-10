#ifndef PROGRAMTHREADFACTORY_HPP
#define PROGRAMTHREADFACTORY_HPP

#include  <vector>

#include    "ProgramThread.hpp"

#include    "../automata/automata.hpp"
#include    "../parse/ast.hpp"
#include    "../parse/parse.hpp"
#include    "../parse/standardize.hpp"
#include    "../automata/reasonning.hpp"

namespace rv_xjtu_yangyan
{
    using namespace std;

    class ProgramThreadFactory
    {
    public:
        ProgramThreadFactory() { }

        void addFile(string filename)
        {
           ifstream in(filename.c_str(), ios_base::in);

           if(!in)
           {
               cerr << "错误：不能打开输入的文件" << filename << endl;
               return;
           }
        
           string storage;
           in.unsetf(ios::skipws);
           copy(
                   istream_iterator<char>(in), 
                   istream_iterator<char>(),
                   back_inserter(storage));
           exLTL_grammar<string::const_iterator> g;
           exLTL_file t;
           
           using boost::spirit::ascii::space;
           string::const_iterator iter = storage.begin();
           string::const_iterator end = storage.end();

           bool r = boost::spirit::qi::phrase_parse(iter, end, g, space, t);

           if(r && iter == end)
           {
               cout << "解析成功\n";
               exLTL_file_analyze printer;
               cout << "输入文本为:\n";
               cout << "------------------------------------------------------------------" << endl;
               printer(t);
               cout << "------------------------------------------------------------------" << endl;
               ast_file *ast;
               ast = exLTL_file_to_ast()(t);
               ast_standardize()(ast);
               cout << "标准化后抽象语法树为" << endl;
               cout << "------------------------------------------------------------------" << endl;
               ast_file_printer()(ast);
               cout << "------------------------------------------------------------------" << endl;
               automata_collection *ac = ast_file_to_automata()(ast);

               //将自动机按照程序名称归类添加
               for(vector<automata_rule *>::iterator it = ac->automata_rules.begin();
                       it != ac->automata_rules.end(); it++)
               {
                   bool inExistedProgram = false;
                   for(vector<ProgramThread *>::iterator itpt = threads.begin();
                           itpt != threads.end(); itpt++)
                   {
                       if((*it)->program_name == (*itpt)->programName)
                       {
                           (*itpt)->insertAutomata(*it);
                           inExistedProgram = true;
                           break;
                       }
                   }
                   if(inExistedProgram == false)
                   {
                       //这里，如果没有找到相应的ProgramThread，那我们需要创建一个新的
                       ProgramThread *pt = new ProgramThread((*it)->program_name);
                       pt->insertAutomata(*it);
                       threads.push_back(pt);
                   }
               }
           }
           else
           {
               cerr <<  "解析失败了，请检查语法有没有问题\n";
           }

        }

        void runAll()
        {
            for(vector<ProgramThread *>::iterator it = threads.begin();
                    it != threads.end(); it++)
            {
                (*it)->run();
            }
        }

        vector<ProgramThread *> threads;
    };

}
#endif

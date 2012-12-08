#ifndef AST_HPP
#define AST_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include    "parse.hpp"

namespace rv_xjtu_yangyan
{
    inline bool is_unary_operator(std::string op)
    {
        if(op == "G"
            || op == "F"
            || op == "N"
            || op == "~"
            || op == "NULL1")
            return true;
        else
            return false;
    }

    inline bool is_binary_operator(std::string op)
    {
        if(op == "U"
            || op == "W"
            || op == "R"
            || op == "|"
            || op == "&"
            || op == "->"
            || op == "NULL2")
            return true;
        else
            return false;
    }

    inline bool is_event(std::string op)
    {
        if(op == "event")
            return true;
        else
            return false;
    }

    struct ast_key_vars
    {
        std::vector<std::string> vars;
    };

    /*
     *struct ast_non_key_vars
     *{
     *    std::vector<std::string> vars;
     *};
     */

    struct ast_op
    {
        std::string op_name;
    };

    struct ast_expr:ast_op
    {
    };

    struct ast_event:ast_expr
    {
        ast_event()
        {
            op_name = "event";
        }
        std::string event_name;
        std::vector<std::string> paras;
    };

    struct ast_unary_expr:ast_expr
    {
        ast_unary_expr()
        {
            op_name = "not_set";
        }
        ast_expr *subexpr;
    };

    struct ast_binary_expr:ast_expr
    {
        ast_binary_expr()
        {
            op_name = "not_set";
        }
        ast_expr *lh_subexpr;
        ast_expr *rh_subexpr;
    };

    struct ast_scope
    {
        ast_scope()
        {
            samelevel = true;
        }
        bool samelevel;//表明end和begin是在同一层的
        ast_event *begin;
        ast_event *end;
    };

    struct ast_solution
    {
        ast_solution() {}

        std::string solution_name;
        std::vector<std::string> paras;
    };

    struct ast_item
    {
        ast_item()
        {
        }

        ast_scope *scope;
        ast_key_vars *keyvars;
        //ast_non_key_vars *nkeyvars;
        ast_expr *expr;
        ast_solution *solution;
    };

    struct ast_rule
    {
        ast_rule()
        {
        }
        std::string program_name;
        std::string rule_name;
        std::vector<ast_item *> items;
    };

    struct ast_file
    {
        ast_file()
        {
        }
        std::vector<ast_rule *> rules;
    };
}

namespace rv_xjtu_yangyan
{
    ////////////////////////////////////////////////
    //将解析树复制到抽象语法树
    ////////////////////////////////////////////////
    struct exLTL_event_to_ast
    {
        exLTL_event_to_ast() { } 
        ast_event *operator()(exLTL_event const &event) const
        {
            ast_event *rvp = new ast_event();
            rvp->event_name = event.event;
            BOOST_FOREACH(exLTL_var const &var, event.para_list.vars)
            {
                rvp->paras.push_back(var.var_name);
            }
            return rvp;
        }
    };

    struct exLTL_unary_expr_to_ast
    {
        exLTL_unary_expr_to_ast() { }
        ast_unary_expr *operator()(exLTL_unary_expr &expr) const;
    };

    struct exLTL_binary_expr_to_ast
    {
        exLTL_binary_expr_to_ast() { }
        ast_binary_expr *operator()(exLTL_binary_expr &expr) const;
    };

    struct exLTL_var_expr_to_ast : boost::static_visitor<ast_expr *>
    {
        exLTL_var_expr_to_ast() { }
        ast_expr *operator()(exLTL_event &expr) const 
        {
            return (ast_expr *) exLTL_event_to_ast()(expr);
        }
        ast_expr *operator()(exLTL_unary_expr &expr) const
        {
            return (ast_expr *) exLTL_unary_expr_to_ast()(expr);
        }
        ast_expr *operator()(exLTL_binary_expr &expr) const
        {
            return (ast_expr *) exLTL_binary_expr_to_ast()(expr);
        }

    };

    ast_unary_expr *exLTL_unary_expr_to_ast::operator()(exLTL_unary_expr &expr) const
    {
        ast_unary_expr *rvp = new ast_unary_expr();
        rvp->op_name = expr.op;
        rvp->subexpr= boost::apply_visitor(exLTL_var_expr_to_ast(), expr.expr);
        return rvp;
    }

    ast_binary_expr *exLTL_binary_expr_to_ast::operator()(exLTL_binary_expr &expr) const
    {
        ast_binary_expr *rvp = new ast_binary_expr();
        rvp->op_name = expr.op;
        rvp->lh_subexpr = boost::apply_visitor(exLTL_var_expr_to_ast(), expr.lh_expr);
        rvp->rh_subexpr = boost::apply_visitor(exLTL_var_expr_to_ast(), expr.rh_expr);
        return rvp;
    }

    struct exLTL_scopelist_to_ast
    {
        exLTL_scopelist_to_ast() { }

        ast_scope *operator()(exLTL_scope_list &scopelist) const
        {
            ast_scope *rvp = new ast_scope();
            if(scopelist.scopes.size() == 0)
            {
                std::cerr << "规则没有作用域" << std::endl;
                exit(-1);
            }
            else if(scopelist.scopes.size() == 1)
            {
                if(scopelist.scopes.at(0).keyword != "within")
                {
                    std::cerr << "作用域使用错误，使用within或者begin与end" << std::endl;
                    exit(-1);
                }
                rvp->samelevel = true;
                rvp->begin = exLTL_event_to_ast()(scopelist.scopes.at(0).event);
                rvp->end = exLTL_event_to_ast()(scopelist.scopes.at(0).event);
            }
            else if(scopelist.scopes.size() == 2)
            {
                std::string first_key = scopelist.scopes.at(0).keyword;
                std::string second_key = scopelist.scopes.at(1).keyword;
                if((first_key  == "begin" && second_key == "end")) 
                {
                    rvp->samelevel = false;
                    rvp->begin = exLTL_event_to_ast()(scopelist.scopes.at(0).event);
                    rvp->end = exLTL_event_to_ast()(scopelist.scopes.at(1).event);
                }
                else if((first_key  == "end" && second_key == "begin")) 
                {
                    rvp->samelevel = false;
                    rvp->end = exLTL_event_to_ast()(scopelist.scopes.at(0).event);
                    rvp->begin = exLTL_event_to_ast()(scopelist.scopes.at(1).event);
                }
                else
                {
                    std::cerr << "作用域使用错误，使用within或者begin与end" << std::endl;
                    exit(-1);
                }
            }
            else
            {
                std::cerr << "作用域使用错误，使用within或者begin与end" << std::endl;
                exit(-1);
            }

            return rvp;
        }
    };

    struct exLTL_keyvars_to_ast
    {
        exLTL_keyvars_to_ast() {}

        ast_key_vars *operator()(exLTL_key_var_list &kvl) const
        {
            ast_key_vars *rvp = new ast_key_vars();
            BOOST_FOREACH(exLTL_var &var, kvl.vars)
            {
                rvp->vars.push_back(var.var_name);
            }
            return rvp;
        }
    };

    struct exLTL_solution_to_ast
    {
        exLTL_solution_to_ast(){}

        ast_solution *operator()(exLTL_solution &solution) const
        {
            ast_solution *rvp = new ast_solution();
            rvp->solution_name = solution.solution_name;
            BOOST_FOREACH(exLTL_var &var, solution.para_list.vars)
            {
                rvp->paras.push_back(var.var_name);
            }
            return rvp;
        }

    };

    struct exLTL_item_to_ast
    {
        exLTL_item_to_ast() { }

        ast_item *operator()(exLTL_item &item) const
        {
            ast_item *rvp = new ast_item();
            rvp->scope = exLTL_scopelist_to_ast()(item.scopelist);
            rvp->keyvars  = exLTL_keyvars_to_ast()(item.keyvarlist);
            rvp->expr = boost::apply_visitor(exLTL_var_expr_to_ast(), item.expr);
            rvp->solution = exLTL_solution_to_ast()(item.solution);
            return rvp;
        }
    };

    struct exLTL_rule_to_ast
    {
        exLTL_rule_to_ast() { }

        ast_rule *operator()(exLTL_rule &rule) 
        {
            ast_rule *rvp = new ast_rule();
            rvp->program_name = rule.program;
            rvp->rule_name = rule.rulename;
            BOOST_FOREACH(exLTL_item &item, rule.items)
            {
                rvp->items.push_back(exLTL_item_to_ast()(item));
            }
            return rvp;
        }
    };

    struct exLTL_file_to_ast
    {
        exLTL_file_to_ast() { } 
        ast_file *operator()(exLTL_file &file) 
        {
            ast_file *rvp = new ast_file();
            BOOST_FOREACH(exLTL_rule &rule, file.rules)
            {
                rvp->rules.push_back(exLTL_rule_to_ast()(rule));
            }
            return rvp;
        }
    };
}

namespace rv_xjtu_yangyan
{
    ///////////////////////////////////////////////////////////
    //ast树的打印---也可以作为解析的模板
    ///////////////////////////////////////////////////////////
    struct ast_event_printer
    {
        ast_event_printer() { }
        void operator()(ast_event *ae)
        {
            std::cout << ae->event_name;
            BOOST_FOREACH(std::string &var, ae->paras)
            {
                std::cout << "#" << var;
            }
        }
    };

    struct ast_unary_expr_printer
    {
        ast_unary_expr_printer() {}
        void operator()(ast_unary_expr *aue);
    };

    struct ast_binary_expr_printer
    {
        ast_binary_expr_printer(){}
        void operator()(ast_binary_expr *abe);
    };

    struct ast_expr_printer
    {
        ast_expr_printer() {}
        void operator()(ast_expr *ae)
        {
            if( is_unary_operator(ae->op_name))
            {
                ast_unary_expr_printer()((ast_unary_expr *)ae);
            }
            else if( is_binary_operator(ae->op_name))
            {
                ast_binary_expr_printer()((ast_binary_expr *)ae);
            }
            else if( is_event(ae->op_name))
            {
                ast_event_printer()((ast_event *)ae);
            }
        }
    };

    void ast_unary_expr_printer::operator()(ast_unary_expr *aue)
    {
        std::cout << "( " << aue->op_name << " ";
        ast_expr_printer()(aue->subexpr);
        std::cout << " )";
    }

    void ast_binary_expr_printer::operator()(ast_binary_expr *abe)
    {
        std::cout << "( ";
        ast_expr_printer()(abe->lh_subexpr);
        std::cout << " " << abe->op_name << " ";
        ast_expr_printer()(abe->rh_subexpr);
        std::cout << " )";
    }

    struct ast_scope_printer
    {
        ast_scope_printer(){}
        void operator()(ast_scope *as)
        {
            if(as->samelevel == true)
            {
                std::cout << "同层次开始和结束点:";
            }
            else
            {
                std::cout << "不同层次开始和结束点:";
            }
            ast_event_printer()(as->begin);
            std::cout << "和";
            ast_event_printer()(as->end);
            std::cout << std::endl;
        }
    };

    struct ast_keyvars_printer
    {
        ast_keyvars_printer() {}
        void operator()(ast_key_vars *akv)
        {
            std::cout << "关键变量是：";
            BOOST_FOREACH(std::string &var, akv->vars)
            {
                std::cout << var << " ";
            }
            std::cout << std::endl;
        }
    };

    struct ast_solution_printer
    {
        ast_solution_printer() { }
        void operator()(ast_solution *as)
        {
            std::cout << as->solution_name;
            BOOST_FOREACH(std::string &var, as->paras)
            {
                std::cout << "#" << var;
            }
        }
    };

    struct ast_item_printer
    {
        ast_item_printer() {}
        void operator()(ast_item *ai)
        {
            ast_scope_printer()(ai->scope);
            ast_keyvars_printer()(ai->keyvars);
            std::cout << "逻辑表达式为：";
            ast_expr_printer()(ai->expr);
            std::cout << std::endl;
            std::cout << "解决方法为：";
            ast_solution_printer()(ai->solution);
            std::cout << std::endl;
            std::cout << std::endl;
        }
    };

    struct ast_rule_printer
    {
        ast_rule_printer() {}
        void operator()(ast_rule *ar)
        {
            for(std::vector<ast_item *>::iterator it = ar->items.begin(); it != ar->items.end(); it++)
            {
                ast_item_printer()(*it);
            }
        }
    };

    struct ast_file_printer
    {
        ast_file_printer() {}
        void operator()(ast_file * af)
        {
            for(std::vector<ast_rule *>::iterator it = af->rules.begin(); it != af->rules.end(); it++)
            {
                ast_rule_printer()(*it);
            }
        }
    };

}
#endif



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

    struct ast_item
    {
        ast_item()
        {
        }

        ast_expr *expr;
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

    struct exLTL_item_to_ast
    {
        exLTL_item_to_ast() { }

        ast_item *operator()(exLTL_item &item) const
        {
            ast_item *rvp = new ast_item();
            rvp->expr = boost::apply_visitor(exLTL_var_expr_to_ast(), item.expr);
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

    struct ast_item_printer
    {
        ast_item_printer() {}
        void operator()(ast_item *ai)
        {
            ast_expr_printer()(ai->expr);
            std::cout << "\n";
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



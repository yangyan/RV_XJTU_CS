#ifndef STANDARDIZE_HPP
#define STANDARDIZE_HPP

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

#include    "ast.hpp"
namespace rv_xjtu_yangyan
{
    ///////////////////////////////////////////////////////////////////////////
    //  语法树标准化
    ///////////////////////////////////////////////////////////////////////////
    struct ast_event_standardize
    {
        ast_event_standardize() { }
        void operator()(ast_event *ae)
        { }
    };

    struct ast_unary_expr_standardize
    {
        ast_unary_expr_standardize() {}
        void operator()(ast_unary_expr *&aue);
    };

    struct ast_binary_expr_standardize
    {
        ast_binary_expr_standardize(){}
        void operator()(ast_binary_expr *&abe);
    };

    struct ast_expr_standardize
    {
        ast_expr_standardize() {}
        void operator()(ast_expr *&ae)
        {
            if( is_unary_operator(ae->op_name))
            {
                ast_unary_expr_standardize()((ast_unary_expr *&)ae);
            }
            else if( is_binary_operator(ae->op_name))
            {
                ast_binary_expr_standardize()((ast_binary_expr *&)ae);
            }
            else if( is_event(ae->op_name))
            {
                ast_event_standardize()((ast_event *)ae);
            }
        }
    };

    void ast_unary_expr_standardize::operator()(ast_unary_expr *&aue)
    {
        //ast_expr_standardize()(aue->subexpr);
        if(aue->op_name == "~")
        {
            if(aue->subexpr->op_name == "~")
            {
                //~~a = a
                ast_unary_expr *temp_aup = (ast_unary_expr *)aue->subexpr;
                aue = (ast_unary_expr *)temp_aup->subexpr;
                ast_expr_standardize()((ast_expr *&)aue);
                return;
            }
            else if(aue->subexpr->op_name == "G")
            {
                //~Ga = F~a
                aue->op_name = "F";
                aue->subexpr->op_name = "~";
                ast_expr_standardize()(aue->subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "F")
            {
                //~Fa = G~a
                aue->op_name = "G";
                aue->subexpr->op_name = "~";
                ast_expr_standardize()(aue->subexpr);
                return;
            }
            else if(aue->subexpr->op_name  == "N")
            {
                //~Na = N~a
                aue->op_name = "N";
                aue->subexpr->op_name = "~";
                ast_expr_standardize()(aue->subexpr);
                return;
            }
            else if(aue->subexpr->op_name  == "X")
            {
                //~Xa = X~a
                aue->op_name = "X";
                aue->subexpr->op_name = "~";
                ast_expr_standardize()(aue->subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "|")
            {
                //~(a|b) = ~a & ~b
                ast_binary_expr *new_abe = new ast_binary_expr();
                ast_unary_expr *new_aue_lh = new ast_unary_expr();
                ast_unary_expr *new_aue_rh = new ast_unary_expr();
                
                new_aue_lh->op_name = "~";
                new_aue_lh->subexpr = ((ast_binary_expr *)aue->subexpr)->lh_subexpr;

                new_aue_rh->op_name = "~";
                new_aue_rh->subexpr = ((ast_binary_expr *)aue->subexpr)->rh_subexpr;

                new_abe->op_name = "&";
                new_abe->lh_subexpr = new_aue_lh;
                new_abe->rh_subexpr = new_aue_rh;

                aue = (ast_unary_expr *)new_abe;
                ast_expr_standardize()(((ast_binary_expr *)aue)->lh_subexpr);
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "&")
            {
                //~(a&b) = ~a | ~b
                ast_binary_expr *new_abe = new ast_binary_expr();
                ast_unary_expr *new_aue_lh = new ast_unary_expr();
                ast_unary_expr *new_aue_rh = new ast_unary_expr();
                
                new_aue_lh->op_name = "~";
                new_aue_lh->subexpr = ((ast_binary_expr *)aue->subexpr)->lh_subexpr;

                new_aue_rh->op_name = "~";
                new_aue_rh->subexpr = ((ast_binary_expr *)aue->subexpr)->rh_subexpr;

                new_abe->op_name = "|";
                new_abe->lh_subexpr = new_aue_lh;
                new_abe->rh_subexpr = new_aue_rh;

                aue = (ast_unary_expr *)new_abe;
                ast_expr_standardize()(((ast_binary_expr *)aue)->lh_subexpr);
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "U")
            {
                //~(a U b) = (~b) W ((~a) & (~b))
                ast_binary_expr *new_abe_W = new ast_binary_expr();
                ast_binary_expr *new_abe_and = new ast_binary_expr();
                ast_unary_expr *new_aue_a = new ast_unary_expr();
                ast_unary_expr *new_aue_b = new ast_unary_expr();

                new_aue_a->op_name = "~";
                new_aue_a->subexpr = ((ast_binary_expr *)aue->subexpr)->lh_subexpr;

                new_aue_b->op_name = "~";
                new_aue_b->subexpr = ((ast_binary_expr *)aue->subexpr)->rh_subexpr;

                new_abe_and->op_name = "&";
                new_abe_and->lh_subexpr = new_aue_a;
                new_abe_and->rh_subexpr = new_aue_b;

                new_abe_W->op_name = "W";
                new_abe_W->lh_subexpr = new_aue_b;
                new_abe_W->rh_subexpr = new_abe_and;

                aue = (ast_unary_expr *)new_abe_W;
                //左边的不用管，因为右边也包含了左边
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "W")
            {
                //~(a W b) = (~b) U ((~a) & (~b))
                ast_binary_expr *new_abe_U = new ast_binary_expr();
                ast_binary_expr *new_abe_and = new ast_binary_expr();
                ast_unary_expr *new_aue_a = new ast_unary_expr();
                ast_unary_expr *new_aue_b = new ast_unary_expr();

                new_aue_a->op_name = "~";
                new_aue_a->subexpr = ((ast_binary_expr *)aue->subexpr)->lh_subexpr;

                new_aue_b->op_name = "~";
                new_aue_b->subexpr = ((ast_binary_expr *)aue->subexpr)->rh_subexpr;

                new_abe_and->op_name = "&";
                new_abe_and->lh_subexpr = new_aue_a;
                new_abe_and->rh_subexpr = new_aue_b;

                new_abe_U->op_name = "U";
                new_abe_U->lh_subexpr = new_aue_b;
                new_abe_U->rh_subexpr = new_abe_and;


                aue = (ast_unary_expr *)new_abe_U;
                //左边的不用管，因为右边也包含了左边
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "R")
            {
                //这个没有测试过
                //~(a R b) = (~ a) U (~b)
                ast_binary_expr *new_abe = new ast_binary_expr();
                ast_unary_expr *new_aue_lh = new ast_unary_expr();
                ast_unary_expr *new_aue_rh = new ast_unary_expr();
                
                new_aue_lh->op_name = "~";
                new_aue_lh->subexpr = ((ast_binary_expr *)aue->subexpr)->lh_subexpr;

                new_aue_rh->op_name = "~";
                new_aue_rh->subexpr = ((ast_binary_expr *)aue->subexpr)->rh_subexpr;

                new_abe->op_name = "U";
                new_abe->lh_subexpr = new_aue_lh;
                new_abe->rh_subexpr = new_aue_rh;

                aue = (ast_unary_expr *)new_abe;
                ast_expr_standardize()(((ast_binary_expr *)aue)->lh_subexpr);
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
            else if(aue->subexpr->op_name == "->")
            {
                //~(a -> b) = a & ~b
                ast_binary_expr *temp_abe = (ast_binary_expr *)aue->subexpr;

                temp_abe->op_name = "&";
                aue->op_name = "~";
                aue->subexpr = temp_abe->rh_subexpr;
                temp_abe->rh_subexpr = aue;
                aue = (ast_unary_expr *)temp_abe;
                ast_expr_standardize()(((ast_binary_expr *)aue)->lh_subexpr);
                ast_expr_standardize()(((ast_binary_expr *)aue)->rh_subexpr);
                return;
            }
        }
        else if(aue->op_name == "G")
        {
            if(aue->subexpr->op_name == "G")
            {
                //GGa = Ga
                ast_expr *temp_aue = aue->subexpr;
                aue = (ast_unary_expr *)temp_aue;
                ast_expr_standardize()((ast_expr *&)aue);
                return;
            }
        }
        else if(aue->op_name == "F")
        {
            if(aue->subexpr->op_name == "F")
            {
                //FFa = Fa
                ast_expr *temp_aue = aue->subexpr;
                aue = (ast_unary_expr *)temp_aue;
                ast_expr_standardize()((ast_expr *&)aue);
                return;
            }
        }
        //default
        ast_expr_standardize()(aue->subexpr);
        return;
    }

    void ast_binary_expr_standardize::operator()(ast_binary_expr *&abe)
    {
        if(abe->op_name == "R")
        {
            // a R b = b W (a & b)
            ast_binary_expr *temp_abe_and = new ast_binary_expr();

            temp_abe_and->op_name = "&";
            temp_abe_and->lh_subexpr = abe->lh_subexpr;
            temp_abe_and->rh_subexpr = abe->rh_subexpr;

            abe->lh_subexpr = abe->rh_subexpr;
            abe->op_name = "W";
            abe->rh_subexpr = temp_abe_and;

            ast_expr_standardize()(abe->rh_subexpr);
            return;
        }
        else if(abe->op_name == "->")
        {
            // a -> b = ~a | b
            ast_unary_expr *new_aue = new ast_unary_expr();

            new_aue->op_name = "~";
            new_aue->subexpr = abe->lh_subexpr;

            abe->lh_subexpr = new_aue;
            abe->op_name = "|";
            ast_expr_standardize()(abe->lh_subexpr);
            ast_expr_standardize()(abe->rh_subexpr);
            return;
        }
            
        ast_expr_standardize()(abe->lh_subexpr);
        ast_expr_standardize()(abe->rh_subexpr);
        return;
    }

    struct ast_item_standardize
    {
        ast_item_standardize() {}
        void operator()(ast_item *ai)
        {
            ast_expr_standardize()(ai->expr);
        }
    };

    struct ast_rule_standardize
    {
        ast_rule_standardize() {}
        void operator()(ast_rule *ar)
        {
            for(std::vector<ast_item *>::iterator it = ar->items.begin(); it != ar->items.end(); it++)
            {
                ast_item_standardize()(*it);
            }
        }
    };

    struct ast_file_standardize
    {
        ast_file_standardize() {}
        void operator()(ast_file * af)
        {
            for(std::vector<ast_rule *>::iterator it = af->rules.begin(); it != af->rules.end(); it++)
            {
                ast_rule_standardize()(*it);
            }
        }
    };

    struct ast_standardize
    {
        void operator()(ast_file *af)
        {
            //采用第二次扫描 ，可以消除冗余
            ast_file_standardize()(af);
            ast_file_standardize()(af);
        }
    };

}

#endif

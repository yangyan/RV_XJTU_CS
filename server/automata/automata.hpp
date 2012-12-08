#ifndef AUTOMATA_HPP
#define AUTOMATA_HPP

#include  <iostream>
#include  <vector>

#include    "../parse/ast.hpp"

namespace rv_xjtu_yangyan
{
    ///////////////////////////////////////////////////////////
    //数据结构的定义
    ///////////////////////////////////////////////////////////

    struct automata_node;
    struct automata_scope;

    struct automata_key_vars
    {
        std::vector<std::string> vars;
    };

    struct automata_nonkey_vars
    {
        std::vector<std::string> vars;
    };

    struct automata_solution
    {
        std::string solution_name;
        std::vector<std::string> paras;
    };

    struct automata_type
    {
        std::string type;
        automata_solution *solution;
        automata_scope *scope;
        automata_key_vars *keyvar;
        automata_nonkey_vars *nonkeyvar;
    };

    struct automata_leaf:automata_type
    {
        automata_leaf()
        {
            type = "leaf";
        }

        void set_event(std::string name, std::vector<std::string> &vars)
        {
            event_name  = name;
            //下面是不变的
            is_negative = false;
            is_true_leaf = false;
            is_acceptable = true;
            next_node = NULL;
            for(std::vector<std::string>::iterator it = vars.begin();
                    it != vars.end(); it++)
            {
                paras.push_back((*it));
            }
        }

        void set_next_node(bool acceptable, automata_node *next)
        {
            is_acceptable = acceptable;
            next_node = next;
            //下面不变 
            is_true_leaf = true;
            is_negative = false;
            event_name = "true";
        }


        bool is_true_leaf;      //区分事件还是true
        bool is_acceptable;     //区分可接受true还是不可接受true
        bool is_negative;       //区分事件
        std::string event_name;
        std::vector<std::string> paras;
        automata_node *next_node; 
    };

    struct automata_node:automata_type
    {
        automata_node()
            :left_result("ambiguous"),right_result("ambiguous")
        {
            type = "node";
        }
        std::string relationship;       //"and" "or"
        std::string left_result;        //"true" "false" "ambiguous"
        std::string right_result;       //"true" "false" "ambiguous"
        automata_type *left_automata;
        automata_type *right_automata;
    };

    struct automata_scope
    {
        bool samelevel;
        automata_leaf *begin;
        automata_leaf *end;
    };

    //表示一个rule结构中的所有自动机
    struct automata_rule
    {
        std::string program_name;
        std::string rule_name;
        std::vector<automata_type *> automatas;
    };

    //表示所有自动机的集合
    struct automata_collection
    {
        std::vector<automata_rule *> automata_rules;
    };

#define LEAF_P(p) ((automata_leaf *)p)
#define NODE_P(p) ((automata_node *)p)
}

namespace rv_xjtu_yangyan
{
    ////////////////////////////////////////////////////////////////////
    //获取一个自动机中所有非关键变量
    ////////////////////////////////////////////////////////////////////
    struct get_nonkey_vars_from_at//automata_type
    {
        get_nonkey_vars_from_at(automata_key_vars *av, automata_nonkey_vars *anv)
            :top_anv_(anv), top_av_(av)
        { 
        }

        void operator()(automata_type *at)
        {
            if(at->type == "leaf" && LEAF_P(at)->is_true_leaf == false)
            {
                automata_leaf *al = LEAF_P(at);
                for(std::vector<std::string>::iterator it = al->paras.begin();
                        it != al->paras.end(); it++)
                {
                    //确保变量不在key中
                    std::vector<std::string>::iterator it0 = 
                        find(top_av_->vars.begin(), top_av_->vars.end(), (*it));
                    if(it0 == top_av_->vars.end())
                    {
                        //确保变量不在nonkey中
                        std::vector<std::string>::iterator it1 = 
                            find(top_anv_->vars.begin(), top_anv_->vars.end(), (*it));
                        if(it1 == top_anv_->vars.end())
                            top_anv_->vars.push_back(*it);
                    }
                }
            }
            else if(at->type == "node")
            {
                get_nonkey_vars_from_at(top_av_, top_anv_)(NODE_P(at)->left_automata);
                get_nonkey_vars_from_at(top_av_, top_anv_)(NODE_P(at)->right_automata);
            }
        }
        automata_nonkey_vars *top_anv_;
        automata_key_vars *top_av_;
    };
    
    ///////////////////////////////////////////////////////////////////////////
    //  标准化后的抽象语法树转化为自动机
    ///////////////////////////////////////////////////////////////////////////
    struct ast_event_to_automata
    {
        ast_event_to_automata() { }
        automata_type *operator()(ast_event *ae)
        {
            automata_leaf *rv_al = new automata_leaf();
            rv_al->set_event(ae->event_name, ae->paras);
            return (automata_type *)rv_al;
        }
    };

    struct ast_unary_expr_to_automata
    {
        ast_unary_expr_to_automata() {}
        automata_type *operator()(ast_unary_expr *aue);
    };

    struct ast_binary_expr_to_automata
    {
        ast_binary_expr_to_automata(){}
        automata_type *operator()(ast_binary_expr *abe);
    };

    struct ast_expr_to_automata
    {
        ast_expr_to_automata() {}
        automata_type *operator()(ast_expr *ae)
        {
            if(is_unary_operator(ae->op_name))
            {
                return ast_unary_expr_to_automata()((ast_unary_expr *)ae);
            }
            else if(is_binary_operator(ae->op_name))
            {
                return ast_binary_expr_to_automata()((ast_binary_expr *)ae);
            }
            else if(is_event(ae->op_name))
            {
                return ast_event_to_automata()((ast_event *)ae);
            }
            return NULL;
        }
    };

    automata_type *ast_unary_expr_to_automata::operator()(ast_unary_expr *aue)
    {
        if(aue->op_name == "~")
        {
            automata_leaf *rv_al = (automata_leaf *)ast_expr_to_automata()(aue->subexpr);
            rv_al->is_negative = true;
            return rv_al;
        }
        else if(aue->op_name == "G")
        {   
            //G a = true(*) & a()
            automata_node *rv_an = new automata_node();

            automata_leaf *true_al = new automata_leaf();
            true_al->set_next_node(true, rv_an);

            rv_an->relationship = "and";
            rv_an->left_automata = true_al;
            rv_an->right_automata = ast_expr_to_automata()(aue->subexpr);
            return rv_an;
        }
        else if(aue->op_name == "F")
        {
            //F a = true | a()
            automata_node *rv_an = new automata_node();

            automata_leaf *true_al = new automata_leaf();
            true_al->set_next_node(false, rv_an);

            rv_an->relationship = "or";
            rv_an->left_automata = true_al;
            rv_an->right_automata = ast_expr_to_automata()(aue->subexpr);
            return rv_an;
        }
            std::cerr << "自动机转化时遇到了没有定义的结点:" << aue->op_name << std::endl;
        return NULL;
    }

    automata_type *ast_binary_expr_to_automata::operator()(ast_binary_expr *abe)
    {
        if(abe->op_name == "&")
        {
            //a & b = a & b 
            automata_node *rv_an = new automata_node();

            rv_an->relationship = "and";
            rv_an->left_automata = ast_expr_to_automata()(abe->lh_subexpr);
            rv_an->right_automata = ast_expr_to_automata()(abe->rh_subexpr);
            return rv_an;
        }
        else if(abe->op_name == "|")
        {
            //a | b = a | b
            automata_node *rv_an = new automata_node();

            rv_an->relationship = "or";
            rv_an->left_automata = ast_expr_to_automata()(abe->lh_subexpr);
            rv_an->right_automata = ast_expr_to_automata()(abe->rh_subexpr);
            return rv_an;
        }
        else if(abe->op_name == "U")
        {
            // a U b = b | (a & true)
            automata_node *rv_an = new automata_node();

            automata_leaf *true_al = new automata_leaf();
            true_al->set_next_node(false, rv_an);

            automata_node *right_an = new automata_node();
            right_an->relationship = "and";
            right_an->left_automata = ast_expr_to_automata()(abe->lh_subexpr);
            right_an->right_automata = true_al;

            rv_an->relationship = "or";
            rv_an->left_automata = ast_expr_to_automata()(abe->rh_subexpr);
            rv_an->right_automata = right_an;
            
            return rv_an;
        }
        else if(abe->op_name == "W")
        {
            // a W b = b | (a & true(*))
            automata_node *rv_an = new automata_node();

            automata_leaf *true_al = new automata_leaf();
            true_al->set_next_node(true, rv_an);

            automata_node *right_an = new automata_node();
            right_an->relationship = "and";
            right_an->left_automata = ast_expr_to_automata()(abe->lh_subexpr);
            right_an->right_automata = true_al;

            rv_an->relationship = "or";
            rv_an->left_automata = ast_expr_to_automata()(abe->rh_subexpr);
            rv_an->right_automata = right_an;
            
            return rv_an;
        }

        std::cerr << "自动机转化时遇到了没有定义的结点:" << abe->op_name << std::endl;
        return NULL;
    }

    struct ast_scope_to_automata
    {
        ast_scope_to_automata() {}
        automata_scope *operator()(ast_scope *as)
        {
            automata_scope *rv;
            rv = new automata_scope();
            rv->samelevel = as->samelevel;
            rv->begin = LEAF_P(ast_event_to_automata()(as->begin));
            rv->end = LEAF_P(ast_event_to_automata()(as->end));
            return rv;
        }
    };
    
    struct ast_key_vars_to_automata
    {
        automata_key_vars *operator()(ast_key_vars *akv)
        {
            automata_key_vars *rv;
            rv = new automata_key_vars();
            for(std::vector<std::string>::iterator it = akv->vars.begin();
                    it != akv->vars.end(); it++)
            {
                rv->vars.push_back((*it));
            }
            return rv;
        }
    };

    struct ast_solution_to_automata
    {
        ast_solution_to_automata() { }
        automata_solution *operator()(ast_solution *as)
        {
            automata_solution *rv_as = new automata_solution();
            rv_as->solution_name = as->solution_name;
            BOOST_FOREACH(std::string &para, as->paras)
            {
                rv_as->paras.push_back(para);
            }
            return rv_as;
        }
    };

    struct ast_item_to_automata
    {
        ast_item_to_automata() {}
        automata_type *operator()(ast_item *ai)
        {
            automata_type *rv;
            rv = ast_expr_to_automata()(ai->expr);
            rv->solution = ast_solution_to_automata()(ai->solution);
            rv->scope = ast_scope_to_automata()(ai->scope);
            rv->keyvar = ast_key_vars_to_automata()(ai->keyvars);
            rv->nonkeyvar = new automata_nonkey_vars();
            get_nonkey_vars_from_at(rv->keyvar, rv->nonkeyvar)(rv);
            BOOST_FOREACH(std::string &var, rv->keyvar->vars)
            {
                std::cout << "关键变量为：" << var << std::endl;
            }
            BOOST_FOREACH(std::string &var, rv->nonkeyvar->vars)
            {
                std::cout << "非关键变量为：" << var << std::endl;
            }
            return rv;
        }
    };

    struct ast_rule_to_automata
    {
        ast_rule_to_automata() {}
        automata_rule *operator()(ast_rule *ar)
        {
            automata_rule *rv_ar = new automata_rule();
            rv_ar->program_name = ar->program_name;
            rv_ar->rule_name = ar->rule_name;
            for(std::vector<ast_item *>::iterator it = ar->items.begin(); it != ar->items.end(); it++)
            {
                rv_ar->automatas.push_back(ast_item_to_automata()(*it));
            }
            return rv_ar;
        }
    };

    struct ast_file_to_automata
    {
        ast_file_to_automata() {}
        automata_collection *operator()(ast_file *af)
        {
            automata_collection *rv_ac = new automata_collection();
            for(std::vector<ast_rule *>::iterator it = af->rules.begin(); it != af->rules.end(); it++)
            {
                automata_rule *ar;
                ar = ast_rule_to_automata()(*it);
                rv_ac->automata_rules.push_back(ar);
            }
            return rv_ac;
        }
    };



} 

#endif

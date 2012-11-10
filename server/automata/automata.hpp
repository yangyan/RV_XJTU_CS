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

    struct automata_type
    {
        std::string type;
    };

    struct automata_leaf:automata_type
    {
        automata_leaf()
        {
            type = "leaf";
        }

        void set_event(std::string name)
        {
            event_name  = name;
            //下面是不变的
            is_negative = false;
            is_true_leaf = false;
            is_acceptable = true;
            next_node = NULL;
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
    ///////////////////////////////////////////////////////////////////////////
    //  标准化后的抽象语法树转化为自动机
    ///////////////////////////////////////////////////////////////////////////
    struct ast_event_to_automata
    {
        ast_event_to_automata() { }
        automata_type *operator()(ast_event *ae)
        {
            automata_leaf *rv_al = new automata_leaf();
            rv_al->set_event(ae->event_name);
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

    struct ast_item_to_automata
    {
        ast_item_to_automata() {}
        automata_type *operator()(ast_item *ai)
        {
            return ast_expr_to_automata()(ai->expr);
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
        //！！！！这里，我们只返回第一个collection，暂时用于调试
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

    ////////////////////////////////////////////////////////////////////
    //获取一个自动机器中所有事件
    ////////////////////////////////////////////////////////////////////

    struct get_events_from_at//automata_type
    {
        get_events_from_at(std::vector<std::string> &events)
            :events_(events)
        {
        }

        void operator()(automata_type *at)
        {
            if(at->type == "leaf" && LEAF_P(at)->is_true_leaf == false)
            {
                events_.push_back(LEAF_P(at)->event_name);
            }
            else if(at->type == "node")
            {
                get_events_from_at((std::vector<std::string> &) events_)(NODE_P(at)->left_automata);
                get_events_from_at((std::vector<std::string> &) events_)(NODE_P(at)->right_automata);
            }
        }

        std::vector<std::string> &events_;
    };
} 

#endif

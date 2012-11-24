#ifndef PARSE_HPP
#define PARSE_HPP

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

namespace rv_xjtu_yangyan
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////
    //  定义抽象语法树的结构
    ///////////////////////////////////////////////////////////////////////////
    
    struct exLTL_binary_expr;
    struct exLTL_unary_expr;
    struct exLTL_event;

    typedef boost::variant<
        exLTL_event,
        boost::recursive_wrapper<exLTL_unary_expr>,
        boost::recursive_wrapper<exLTL_binary_expr>
        >
    exLTL_var_expr;

    struct exLTL_event
    {
        std::string event;
    };

    struct exLTL_unary_expr
    {
        std::string op;
        exLTL_var_expr expr;
    };

    struct exLTL_binary_expr
    {
        std::string op;
        exLTL_var_expr lh_expr;
        exLTL_var_expr rh_expr;
    };

    struct exLTL_item
    {
        exLTL_var_expr expr;
        std::string solution;
    };

    struct exLTL_rule
    {
        std::string program; //program name  
        std::string rulename; //rule name 
        std::vector<exLTL_item> items;
    };
    
    struct exLTL_file
    {
        std::vector<exLTL_rule> rules;
    };

}

// We need to tell fusion about our struct
// to make it a first-class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_file,
        (std::vector<rv_xjtu_yangyan::exLTL_rule>, rules)
)
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_rule,
        (std::string, program)
        (std::string, rulename)
        (std::vector<rv_xjtu_yangyan::exLTL_item>, items)
)
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_item,
        (rv_xjtu_yangyan::exLTL_var_expr, expr)
        (std::string, solution)
)
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_binary_expr,
        (rv_xjtu_yangyan::exLTL_var_expr, lh_expr)
        (std::string, op)
        (rv_xjtu_yangyan::exLTL_var_expr, rh_expr)
)
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_unary_expr,
        (std::string, op)
        (rv_xjtu_yangyan::exLTL_var_expr, expr)
)
BOOST_FUSION_ADAPT_STRUCT(
        rv_xjtu_yangyan::exLTL_event,
        (std::string, event)
)

namespace rv_xjtu_yangyan
{
    ///////////////////////////////////////////////////////////////////////////
    //  语法树打印和解析，主要用于测试和模板
    ///////////////////////////////////////////////////////////////////////////
    int const tabsize = 1;
    void tab(int indent)
    {
        for (int i = 0; i < indent; ++i)
            std::cout << ' ';
    }

    struct exLTL_event_analyze
    {
        exLTL_event_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_event const &event) const
        {
            std::cout << event.event;
        }

        int indent;
    };

    struct exLTL_unary_expr_analyze
    {
        exLTL_unary_expr_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_unary_expr const &expr) const;

        int indent;
    };

    struct exLTL_binary_expr_analyze
    {
        exLTL_binary_expr_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_binary_expr const &expr) const;

        int indent;
    };



    struct exLTL_var_expr_analyze : boost::static_visitor<>
    {
        //exLTL_var_expr_analyze具体实现
        exLTL_var_expr_analyze(int indent = 0)
            : indent(indent)
        {
        }
        void operator()(exLTL_event const &expr) const 
        {
            exLTL_event_analyze((int)indent)(expr);
        }
        void operator()(exLTL_unary_expr const &expr) const
        {
            exLTL_unary_expr_analyze((int)indent)(expr);
        }
        void operator()(exLTL_binary_expr const &expr) const
        {
            exLTL_binary_expr_analyze((int)indent)(expr);
        }

        int indent;
    };

    void exLTL_unary_expr_analyze::operator()(exLTL_unary_expr const &expr) const
    {
        std::cout << "(";
        std::cout << expr.op << " ";
        boost::apply_visitor(exLTL_var_expr_analyze(indent+tabsize), expr.expr);
        std::cout << ")";
    }

    void exLTL_binary_expr_analyze::operator()(exLTL_binary_expr const &expr) const
    {
        std::cout << "(";
        boost::apply_visitor(exLTL_var_expr_analyze(indent+tabsize), expr.lh_expr);
        std::cout << " " << expr.op << " ";
        boost::apply_visitor(exLTL_var_expr_analyze(indent+tabsize), expr.rh_expr);
        std::cout << ")";
    }



    struct exLTL_item_analyze
    {
        exLTL_item_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_item const &item) const
        {
            boost::apply_visitor(exLTL_var_expr_analyze(indent+tabsize), item.expr);
            std::cout << "---- Solution is " << item.solution;
        }

        int indent;
    };

    struct exLTL_rule_analyze
    {
        exLTL_rule_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_rule const &rule) 
        {
            tab(indent);
            std::cout << "Program Name:" << rule.program << " ";
            tab(indent);
            std::cout << "Rule Name:" << rule.rulename << std::endl;

            BOOST_FOREACH(exLTL_item const &item, rule.items)
            {
                exLTL_item_analyze(indent+tabsize)(item);
                std::cout << std::endl;
            }
        }

        int indent;
    };

    struct exLTL_file_analyze
    {
        exLTL_file_analyze(int indent = 0)
            : indent(indent)
        {
        }

        void operator()(exLTL_file &file) 
        {
            tab(indent);
            BOOST_FOREACH(exLTL_rule const &rule, file.rules)
            {
                exLTL_rule_analyze(indent+tabsize)(rule);
            }
            std::cout << std::endl;
        }

        int indent;
    };

}

namespace rv_xjtu_yangyan
{

    ///////////////////////////////////////////////////////////////////////////
    //  exLTL的语法在此定义
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct exLTL_grammar
      : qi::grammar<Iterator, exLTL_file(), ascii::space_type>
    {
        exLTL_grammar()
          : exLTL_grammar::base_type(file, "file")
        {
            using qi::lit;
            using qi::lexeme;
            using qi::on_error;
            using qi::fail;
            using qi::eps;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;

            using phoenix::construct;
            using phoenix::val;

            ////////////////////////////////////////////////////////////////
            // 语法规则
            ////////////////////////////////////////////////////////////////
            //f表示函数，v表示变量

            event_str = char_("fv") >> char_('_') >> +char_("a-zA-Z_0-9");
            event = '"' >> event_str >> '"'; 
            var_expr = unary_expr | binary_expr | event; //event必须在最后

            unary_expr =  char_("GFN~") >> 
                (event | '(' >> var_expr >> ')');

            binary_expr = (event | '(' >> var_expr >> ')')
                >> (char_("UWR|&") | char_("-") >> char_(">"))
                >> (event | '(' >> var_expr >> ')');

            solution_str = +char_("a-zA-Z_0-9");

            item = var_expr >> ':' >> solution_str >> ';';

            name_str = +char_("a-zA-Z_0-9");

            rule = "Rule"
                >> name_str >> '(' >> name_str >> ')'
                >> '{' >> *item >> '}';

            file = eps >> *rule;

            ////////////////////////////////////////////////////////////////
            //命名
            ////////////////////////////////////////////////////////////////
            event.name("event");
            event_str.name("event_str");
            unary_expr.name("unary_expr");
            binary_expr.name("binary_expr");
            var_expr.name("var_expr");
            item.name("item");
            rule.name("rule");
            file.name("file");

            /*
             *debug(event);
             *debug(event_str);
             *debug(unary_expr);
             *debug(binary_expr);
             *debug(var_expr);
             *debug(item);
             *debug(rule);
             *debug(file);
             */
        }

        qi::rule<Iterator, exLTL_file(), ascii::space_type> file;
        qi::rule<Iterator, exLTL_rule(), ascii::space_type> rule;
        qi::rule<Iterator, exLTL_item(), ascii::space_type> item;
        qi::rule<Iterator, exLTL_var_expr(), ascii::space_type> var_expr;
        qi::rule<Iterator, exLTL_binary_expr(), ascii::space_type> binary_expr;
        qi::rule<Iterator, exLTL_unary_expr(), ascii::space_type> unary_expr;
        qi::rule<Iterator, exLTL_event(), ascii::space_type> event;
        qi::rule<Iterator, std::string(), ascii::space_type> event_str;
        qi::rule<Iterator, std::string(), ascii::space_type> name_str;
        qi::rule<Iterator, std::string(), ascii::space_type> solution_str;
    };
    //]
}

#endif



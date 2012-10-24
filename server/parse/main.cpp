/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A mini XML-like parser
//
//  [ JDG March 25, 2007 ]   spirit2
//
///////////////////////////////////////////////////////////////////////////////

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
        exLTL_var_expr lh_expr;
        std::string op;
        exLTL_var_expr rh_expr;
    };

    struct exLTL_item
    {
        exLTL_var_expr expr;
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
                >> (char_("UW|&") | char_("-") >> char_(">"))
                >> (event | '(' >> var_expr >> ')');

            item = var_expr >> ';';

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

            debug(event);
            debug(event_str);
            debug(unary_expr);
            debug(binary_expr);
            debug(var_expr);
            debug(item);
            debug(rule);
            debug(file);
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
    };
    //]
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    rv_xjtu_yangyan::exLTL_grammar<std::string::const_iterator> g; // Our grammar
    rv_xjtu_yangyan::exLTL_file ast; // Our tree

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = boost::spirit::qi::phrase_parse(iter, end, g, space, ast);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
        return 0;
    }
    else
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
        return 1;
    }
}



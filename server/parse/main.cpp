#include    "parse.hpp"
#include    "ast.hpp"
#include    "standardize.hpp"
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
        rv_xjtu_yangyan::exLTL_file_analyze printer;
        printer(ast);
        rv_xjtu_yangyan::ast_file *file;
        file = rv_xjtu_yangyan::exLTL_file_to_ast()(ast);
        rv_xjtu_yangyan::ast_file_standardize()(file);
        rv_xjtu_yangyan::ast_file_printer()(file);
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



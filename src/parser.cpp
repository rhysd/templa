#include <stdexcept>

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_as.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "parser.hpp"

namespace templa {
namespace syntax {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

using qi::_val;
using qi::lit;
using qi::_1;
using qi::_2;
using phx::bind;

template<class Iterator>
class grammar : qi::grammar<Iterator, ast::ast_node(), ascii::space_type> {
    template<class Value>
    using rule = qi::rule<Iterator, Value, ascii::space_type>;

public:
    grammar() : grammar::base_type(root)
    {

        formula[_val = _1]
            = (lit("+")[bind(&ast::formula::maybe_sign, ast::formula::sign::plus)]
            | lit("-")[bind(&ast::formula::maybe_sign, ast::formula::sign::minus)])
            >> ( term[bind(&ast::ast_node::value, _val)] % additive_operator
            ) [bind(&ast::formula::terms, _val)];

        term[_val = _1]
            = (factor % mult_operator)[bind(&ast::ast_node::value, _val)];

        factor
            = ("!" >> factor
            | "(" >> expression >> ")"
            | constant
            | func_call)[bind(&ast::factor::value, _val)];

        relational_operator
            = (lit("==")
                 | "!="
                 | "<"
                 | ">"
                 | "<="
                 | ">=") [bind(&ast::relational_operator::value, _val)];

        additive_operator
            = (lit("+")
                 | "-"
                 | "|"
                 | "||") [bind(&ast::additive_operator::value, _val)];

        mult_operator
            = (lit("*")
                 | "/"
                 | "%"
                 | "&"
                 | "&&") [bind(&ast::mult_operator::value, _val)];

        constant[_val = _1]
            = (qi::int_
             | qi::char_
             | qi::bool_
             | '"' > qi::string > '"')[bind(&ast::constant::value, _val)]
             | list [bind(&ast::ast_node::value, _val)];

        list = (enum_list | int_list | char_list)[bind(&ast::list::value, _val)];

        enum_list[_val = _1]
            = "["
            >> (expression % ",")[bind(&ast::ast_node::value, _val)]
            > "]";

        int_list
            = "["
            >> (qi::int_
            >> ".."
            >> qi::int_)[bind([](int const min, int const max){ return ast::int_list{min, max}; }, _1, _2)]
            >> "]";

        char_list
            = ("["
            >> qi::char_
            >> ".."
            >> qi::char_
            >> "]")[bind([](char const min, char const max){ return ast::char_list{min, max}; }, _1, _2)];

        func_call[_val = _1]
            = qi::string[bind(&ast::func_call::function_name, _val)]
            >> -("("
            >> call_args[bind(&ast::ast_node::value, _val)]
            >> ")");

        call_args[_val = _1]
            = (expression % ",")[bind(&ast::ast_node::value, _val)];

        qi::on_error<qi::fail>
        (
            root,
            // qi::_2 : end of string to parse
            // qi::_3 : iterator at failed point
            // qi::_4 : what failed?
            std::cerr
                << phx::val( "Error! Expecting " )
                << qi::_4
                << phx::val( " here: \"" )
                << phx::construct<std::string>( qi::_3, _2 )
                << phx::val( "\"" )
                << std::endl
        );
    }

private:
    rule<ast::ast_node()> root;
    rule<ast::program()> program;
    rule<ast::decl_func()> decl_func;
    rule<ast::decl_params()> decl_params;
    rule<ast::decl_param()> decl_param;
    rule<ast::list_match()> list_match;
    rule<ast::type_match()> type_match;
    rule<ast::statement()> statement;
    rule<ast::let_statement()> let_statement;
    rule<ast::if_statement()> if_statement;
    rule<ast::case_statement()> case_statement;
    rule<ast::case_when()> case_when;
    rule<ast::expression()> expression;
    rule<ast::formula()> formula;
    rule<ast::term()> term;
    rule<ast::factor()> factor;
    rule<ast::relational_operator()> relational_operator;
    rule<ast::additive_operator()> additive_operator;
    rule<ast::mult_operator()> mult_operator;
    rule<ast::constant()> constant;
    rule<ast::list()> list;
    rule<ast::enum_list()> enum_list;
    rule<ast::int_list()> int_list;
    rule<ast::char_list()> char_list;
    rule<ast::func_call()> func_call;
    rule<ast::call_args()> call_args;
};

ast::ast parser::parse(std::string const& code)
{
    auto itr = std::begin(code);
    auto const end = std::cend(code);
    grammar<decltype(itr)> spiritual_parser;
    ast::ast_node root;

    // if(qi::phrase_parse(itr, end, spiritual_parser, ascii::space, root) && itr == end) {
    //     return {root};
    // } else {
        throw std::runtime_error("Syntax error:");
    // }
}

} // namespace syntax
} // namespace templa


#include <stdexcept>

#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "phoenix_helper.hpp"
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
using templa::phoenix::make_ast_node;

template<class Iterator>
class grammar : qi::grammar<Iterator, ast::ast_node(), ascii::space_type> {
    template<class Value>
    using rule = qi::rule<Iterator, Value, ascii::space_type>;

public:
    grammar() : grammar::base_type(root)
    {
        relational_operator[_val = _1] = (lit("==") | "!=" | "<" | ">" | "<=" | ">=") [_val = make_ast_node<ast::relational_operator>(_1)];
        additive_operator[_val = _1] = (lit("+") | "-" | "|" | "||") [_val = make_ast_node<ast::additive_operator>(_1) ];
        mult_operator[_val = _1] = (lit("*") | "/" | "%" | "&" | "&&") [_val = make_ast_node<ast::mult_operator>(_1)];

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


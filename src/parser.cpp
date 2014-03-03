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
using qi::_3;
using phx::bind;

namespace detail {

    template<class NodeType>
    struct construct_node {
        template<class... Args>
        NodeType operator()(Args &&... args) const
        {
            return {args...};
        }
    };

} // namespace detail

template<class NodeType, class... Holders>
inline auto bind_node(Holders &&... holders)
{
    return phx::bind(detail::construct_node<NodeType>{}, holders...);
}

template<class Iterator>
class grammar : public qi::grammar<Iterator, ast::ast_node(), ascii::space_type> {
    template<class Value>
    using rule = qi::rule<Iterator, Value, ascii::space_type>;

public:
    grammar() : grammar::base_type(program)
    {
        program
            = (
                decl_func % "\n"
            ) [
                bind_node<ast::program>(_1)
            ]
        ;

        decl_func
            = (
                name
                >> -('(' >> decl_params >> ')')
                >> '='
                >> statement
            ) [
                bind_node<ast::decl_func>(_1, _2, _3)
            ]
        ;

        decl_params
            = (
                decl_param % ","
            ) [
                bind_node<ast::decl_params>(_1)
            ]
        ;

        decl_param
            = (
                  list_match
                | type_match
                | name
            ) [
                bind_node<ast::decl_param>(_1)
            ]
        ;

        list_match
            = (
                +(name > ":")
                > name
            ) [
                bind_node<ast::list_match>(_1, _2)
            ]
        ;

        type_match
            = (
                name > "::" > name
            ) [
                bind_node<ast::type_match>(_1, _2)
            ]
        ;

        statement
            = (
                ( let_statement
                | if_statement
                | case_statement
                | expression
                ) >> "\n"
            ) [
                bind_node<ast::statement>(_1)
            ]
        ;

        let_statement
            = (
                "let"
                >> (decl_func % "\n")
                >> "in"
            ) [
                bind_node<ast::let_statement>(_1)
            ]
        ;

        if_statement
            = (
                "if"
                >> expression
                >> "then"
                >> expression
                >> "else"
                >> expression
            ) [
                bind_node<ast::if_statement>(_1, _2, _3)
            ]
        ;

        case_statement
            = (
                "case"
                >> *case_when
                >> "|"
                >> "otherwise"
                >> expression
            ) [
                bind_node<ast::case_statement>(_1, _2)
            ]
        ;

        case_when
            = (
                "|"
                >> expression
                >> "then"
                >> expression
            ) [
                bind_node<ast::case_when>(_1, _2)
            ]
        ;

        expression
            = (
                // TODO: Get operator
                formula % relational_operator
            ) [
                bind_node<ast::expression>(_1)
            ]
        ;

        // FIXME: Too dirty
        formula
            = (
                -( qi::char_('+') | qi::char_('-') )
                >> (term % additive_operator)
            ) [
                bind(
                    [](auto const& maybe_sign_char
                     , auto const& terms)
                        -> ast::formula
                    {
                        if (maybe_sign_char) {
                            auto const& sign_char = *maybe_sign_char;
                            return {sign_char == '+' ? ast::formula::sign::plus : ast::formula::sign::minus, terms};
                        } else {
                            return {boost::none, terms};
                        }
                    }, _1, _2)
            ]
        ;

        term
            = (
                factor % mult_operator
            ) [bind_node<ast::term>(_1)]
        ;

        factor
            = (
                  "!" >> factor
                | "(" >> expression >> ")"
                | constant
                | func_call
            ) [bind_node<ast::factor>(_1)]
        ;

        relational_operator
            = (
                qi::as_string[lit("==")
                 | "!="
                 | "<"
                 | ">"
                 | "<="
                 | ">="]
              ) [bind_node<ast::relational_operator>(_1)]
        ;

        additive_operator
            = (
                qi::as_string[lit("+")
                 | "-"
                 | "|"
                 | "||"]
              ) [bind_node<ast::additive_operator>(_1)]
        ;

        mult_operator
            = (
                qi::as_string[(lit("*")
                 | "/"
                 | "%"
                 | "&"
                 | "&&")]
            ) [bind_node<ast::mult_operator>(_1)]
        ;

        constant
            = (
                  qi::int_
                | ('\'' > qi::char_ > '\'')
                | qi::bool_
                | qi::as_string[qi::lexeme['"' > *(qi::char_ - '"') > '"']]
                | list
            ) [
                bind_node<ast::constant>(_1)
            ]
        ;

        list
            = (
                enum_list | int_list | char_list
            ) [
                bind_node<ast::list>(_1)
            ]
        ;

        enum_list
            = (
                '['
                >> expression % ','
                > ']'
            ) [
                bind_node<ast::enum_list>(_1)
            ]
        ;

        int_list
            = (
                '['
                >> qi::int_
                >> ".."
                >> qi::int_
                >> ']'
            ) [
                bind_node<ast::int_list>(_1, _2)
            ]
        ;

        char_list
            = (
                '['
                >> qi::char_
                >> ".."
                >> qi::char_
                >> ']'
            ) [
                bind_node<ast::char_list>(_1, _2)
            ]
        ;

        func_call
            = (
                qi::as_string[+qi::char_]
                >> -(
                    '('
                    >> call_args
                    >> ')')
            ) [
                bind_node<ast::func_call>(_1, _2)
            ]
        ;

        call_args
            = (
                expression % ','
            ) [
                bind_node<ast::call_args>(_1)
            ]
        ;

        name = qi::as_string[qi::char_("a-zA-Z_") >> *(qi::alnum | qi::char_('_'))];

        qi::on_error<qi::fail>
        (
            program,
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
    rule<ast::ast_node()> program
                        , decl_func
                        , decl_params
                        , decl_param
                        , list_match
                        , type_match
                        , statement
                        , let_statement
                        , if_statement
                        , case_statement
                        , case_when
                        , expression
                        , formula
                        , term
                        , factor
                        , relational_operator
                        , additive_operator
                        , mult_operator
                        , constant
                        , list
                        , enum_list
                        , int_list
                        , char_list
                        , func_call
                        , call_args;
    rule<std::string()> name;
};

ast::ast parser::parse(std::string const& code)
{
    auto itr = std::begin(code);
    auto const end = std::end(code);
    grammar<decltype(itr)> spiritual_parser;
    ast::ast_node root;

    if (!qi::phrase_parse(itr, end, spiritual_parser, ascii::space, root) || itr != end) {
        throw std::runtime_error("Syntax error:");
    }
    return {root};
}

} // namespace syntax
} // namespace templa


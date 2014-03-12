#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE 1

#include <utility>

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
        ast::ast_node operator()(Args &&... args) const
        {
            return {NodeType{args...}, 0, 0};
        }
    };

} // namespace detail

template<class NodeType, class... Holders>
inline auto bind_node(Holders &&... holders)
{
    return phx::bind(detail::construct_node<NodeType>{}, holders...);
}

namespace detail {
    template<class Iterator>
    auto position_of(Iterator begin, Iterator current)
    {
        std::size_t line = 1;
        std::size_t col = 1;
        for (; begin != current; ++begin) {
            if (*begin == '\n') {
                col = 1;
                line++;
                continue;
            }
            col++;
        }
        return std::make_pair(line, col);
    }
} // namespace detail

template<class Iterator>
class grammar : public qi::grammar<Iterator, ast::ast_node(), ascii::blank_type> {
    template<class Value>
    using rule = qi::rule<Iterator, Value, ascii::blank_type>;

public:
    grammar() : grammar::base_type(program)
    {
        // TODO: empty line should be ignored by skipper
        program
            = (
                decl_func % '\n' > (qi::eol | qi::eoi)
            ) [
                _val = bind_node<ast::program>(_1)
            ]
        ;

        decl_func
            = (
                name
                >> -('(' >> decl_params >> ')')
                >> '='
                >> expression
            ) [
                _val = bind_node<ast::decl_func>(_1, _2, _3)
            ]
        ;

        decl_params
            = (
                decl_param % ','
            ) [
                _val = bind_node<ast::decl_params>(_1)
            ]
        ;

        decl_param
            = (
                  list_match
                | type_match
                | constant
                | name
            ) [
                _val = bind_node<ast::decl_param>(_1)
            ]
        ;

        list_match
            = (
                +(name >> ':')
                >> name
            ) [
                _val = bind_node<ast::list_match>(_1, _2)
            ]
        ;

        type_match
            = (
                name >> "::" >> name
            ) [
                _val = bind_node<ast::type_match>(_1, _2)
            ]
        ;

        expression
            = (
                  let_expression
                | if_expression
                | case_expression
                | primary_expression
            ) [
                _val = bind_node<ast::expression>(_1)
            ]
        ;

        let_expression
            = (
                "let" >> -lit('\n')
                >> decl_func % '\n' >> -lit('\n')
                >> "in" >> -lit('\n')
                >> expression
            ) [
                _val = bind_node<ast::let_expression>(_1, _2)
            ]
        ;

        if_expression
            = (
                "if"
                >> expression
                >> "then" >> -lit('\n')
                >> expression >> -lit('\n')
                >> "else" >> -lit('\n')
                >> expression
            ) [
                _val = bind_node<ast::if_expression>(_1, _2, _3)
            ]
        ;

        case_expression
            = (
                "case" >> lit('\n')
                >> *(case_when >> '\n')
                >> "|" >> "otherwise" >> -lit('\n')
                >> expression
            ) [
                _val = bind_node<ast::case_expression>(_1, _2)
            ]
        ;

        case_when
            = (
                "|" >> expression >> -(lit('\n'))
                >> "then" >> -(lit('\n'))
                >> expression
            ) [
                _val = bind_node<ast::case_when>(_1, _2)
            ]
        ;

        primary_expression
            = (
                // TODO: Get operator
                formula % relational_operator
            ) [
                _val = bind_node<ast::primary_expression>(_1)
            ]
        ;

        // FIXME: Too dirty
        formula
            = (
                -( qi::char_('+') | qi::char_('-') )
                >> (term % additive_operator)
            ) [
                _val = bind_node<ast::formula>(_1, _2)
            ]
        ;

        term
            = (
                factor % mult_operator
            ) [
                _val = bind_node<ast::term>(_1)
            ]
        ;

        factor
            = (
                  '!' >> factor
                | '(' >> primary_expression >> ')'
                | constant
                | func_call
            ) [
                _val = bind_node<ast::factor>(_1)
            ]
        ;

        relational_operator
            = (
                qi::as_string[lit("==")
                 | "!="
                 | '<'
                 | '>'
                 | "<="
                 | ">="]
              ) [
                _val = bind_node<ast::relational_operator>(_1)
            ]
        ;

        additive_operator
            = (
                qi::as_string[lit("+")
                 | '-'
                 | '|'
                 | "||"]
              ) [
                _val = bind_node<ast::additive_operator>(_1)
            ]
        ;

        mult_operator
            = (
                qi::as_string[(lit('*')
                 | '/'
                 | '%'
                 | '&'
                 | "&&")]
            ) [
                _val = bind_node<ast::mult_operator>(_1)
            ]
        ;

        constant
            = (
                  qi::int_
                | ('\'' > qi::char_ > '\'')
                | qi::bool_
                | qi::as_string[qi::lexeme['"' > *(qi::char_ - '"') > '"']]
                | list
            ) [
                _val = bind_node<ast::constant>(_1)
            ]
        ;

        list
            = (
                enum_list | int_list | char_list
            ) [
                _val = bind_node<ast::list>(_1)
            ]
        ;

        enum_list
            = (
                '['
                >> primary_expression % ','
                > ']'
            ) [
                _val = bind_node<ast::enum_list>(_1)
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
                _val = bind_node<ast::int_list>(_1, _2)
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
                _val = bind_node<ast::char_list>(_1, _2)
            ]
        ;

        func_call
            = (
                name
                >> -(
                    '('
                    >> call_args
                    >> ')'
                )
            ) [
                _val = bind_node<ast::func_call>(_1, _2)
            ]
        ;

        call_args
            = (
                primary_expression % ','
            ) [
                _val = bind_node<ast::call_args>(_1)
            ]
        ;

        name
            =
                qi::as_string[(qi::alpha | qi::char_('_'))
                >> *(qi::alnum | qi::char_('_'))]
        ;

        qi::on_error<qi::fail>
        (
            program,
            // qi::_2 : end of string to parse
            // qi::_3 : iterator at failed point
            // qi::_4 : what failed?
            std::cerr
                << phx::val( "Error! Expecting " )
                << qi::_4
                << phx::val( "\nhere:\n\"" )
                << phx::construct<std::string>( _3, _2 ) // TODO: get line and col from iterators
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
                        , expression
                        , let_expression
                        , if_expression
                        , case_expression
                        , case_when
                        , primary_expression
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

    if (!qi::phrase_parse(itr, end, spiritual_parser, ascii::blank, root) || itr != end) {
        auto const pos = detail::position_of(std::begin(code), itr);
        throw parse_error{pos.first, pos.second};
    }

    return {root};
}

std::ostringstream parse_error::buffer;
char const* parse_error::what() const noexcept
{
    auto &buf = parse_error::buffer;
    buf.str() = "";
    buf << std::runtime_error::what() << "line " << line << ", col " << col;
    return buf.str().c_str();
}

} // namespace syntax
} // namespace templa


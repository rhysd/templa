#if !defined TEMPLA_AST_HPP_INCLUDED
#define      TEMPLA_AST_HPP_INCLUDED

#include <vector>
#include <string>
#include <cstddef>

#include <boost/optional.hpp>
#include <boost/mpl/vector/vector30.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

namespace templa {
namespace ast {

struct program;
struct decl_func;
struct decl_params;
struct list_match;
struct type_match;
struct statement;
struct let_statement;
struct if_statement;
struct case_statement;
struct case_when;
struct expression;
struct formula;
struct term;
struct factor;
struct relational_operator;
struct additive_operator;
struct mult_operator;
struct constant;
struct list;
struct enum_list;
struct int_list;
struct char_list;
struct func_call;
struct call_args;

class ast_node {
public:

    // use recursive_wrapper even if the type doesn't have ast_node value
    // because held types can't be incomplete type.
    using node_types = boost::mpl::vector24< boost::recursive_wrapper<program>
                                           , boost::recursive_wrapper<decl_func>
                                           , boost::recursive_wrapper<decl_params>
                                           , boost::recursive_wrapper<list_match>
                                           , boost::recursive_wrapper<type_match>
                                           , boost::recursive_wrapper<statement>
                                           , boost::recursive_wrapper<let_statement>
                                           , boost::recursive_wrapper<if_statement>
                                           , boost::recursive_wrapper<case_statement>
                                           , boost::recursive_wrapper<case_when>
                                           , boost::recursive_wrapper<expression>
                                           , boost::recursive_wrapper<formula>
                                           , boost::recursive_wrapper<term>
                                           , boost::recursive_wrapper<factor>
                                           , boost::recursive_wrapper<relational_operator>
                                           , boost::recursive_wrapper<additive_operator>
                                           , boost::recursive_wrapper<mult_operator>
                                           , boost::recursive_wrapper<constant>
                                           , boost::recursive_wrapper<list>
                                           , boost::recursive_wrapper<enum_list>
                                           , boost::recursive_wrapper<int_list>
                                           , boost::recursive_wrapper<char_list>
                                           , boost::recursive_wrapper<func_call>
                                           , boost::recursive_wrapper<call_args> >;

    using value_type = typename boost::make_variant_over< node_types >::type;

    std::size_t line;
    std::size_t col;
    value_type value;
};

struct program{
    std::vector<ast_node> function_declarations;
};

struct decl_func{
    std::string function_name;
    boost::optional<ast_node> maybe_declaration_params;
    ast_node statement;
};

struct decl_params{
    std::vector<ast_node> declaration_params;
};

struct list_match{
    std::vector<std::string> elements;
    std::string rest_elems_name;
};

struct type_match{
    std::string param_name;
    std::string type_name;
};

struct statement{
    ast_node value;
};

struct let_statement{
    std::vector<ast_node> function_declarations;
};

struct if_statement{
    ast_node condition;
    ast_node expression_if_true;
    ast_node expression_if_false;
};

struct case_statement{
    std::vector<ast_node> case_when;
    ast_node otherwise_expression;
};

struct case_when{
    ast_node condition;
    ast_node then_expression;
};

struct expression{
    std::vector<ast_node> formulae;
};

struct formula{
    enum struct sign {
        minus = 1,
        plus
    };
    boost::optional<sign> sign;
    std::vector<ast_node> terms;
};

struct term{
    std::vector<ast_node> factors;
};

struct factor{
    ast_node value;
};

struct relational_operator{
    std::string value;
};

struct additive_operator{
    std::string value;
};

struct mult_operator{
    std::string value;
};

struct constant{
    boost::variant< int
                    , char
                    , bool
                    , std::string
                    , ast_node > value;
};

struct list{
    ast_node value;
};

struct enum_list{
    std::vector<ast_node> elements;
};

struct int_list{
    int min;
    int max;
};

struct char_list{
    char begin;
    char end;
};

struct func_call{
    std::string function_name;
    boost::optional<ast_node> maybe_call_arguments;
};

struct call_args{
    std::vector<ast_node> arguments;
};


} // namespace ast
} // namespace templa

#endif    // TEMPLA_AST_HPP_INCLUDED

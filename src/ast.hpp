#if !defined TEMPLA_AST_HPP_INCLUDED
#define      TEMPLA_AST_HPP_INCLUDED

#include <vector>
#include <string>
#include <cstddef>

#include <boost/optional.hpp>
#include <boost/mpl/vector/vector30.hpp>
#include <boost/mpl/lambda.hpp>
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
    template<class T>
    struct make_recursive_wrapper {
        using type = boost::recursive_wrapper<T>;
    };

public:

    bool operator==(ast_node const& rhs) const;
    bool operator!=(ast_node const& rhs) const;

    // use recursive_wrapper even if the type doesn't have ast_node value
    // because held types can't be incomplete type.
    using value_type =
        typename boost::make_variant_over<
            typename boost::mpl::transform<
                boost::mpl::vector24<
                    program,
                    decl_func,
                    decl_params,
                    list_match,
                    type_match,
                    statement,
                    let_statement,
                    if_statement,
                    case_statement,
                    case_when,
                    expression,
                    formula,
                    term,
                    factor,
                    relational_operator,
                    additive_operator,
                    mult_operator,
                    constant,
                    list,
                    enum_list,
                    int_list,
                    char_list,
                    func_call,
                    call_args
                >,
                typename boost::mpl::lambda<
                    make_recursive_wrapper<boost::mpl::_1>
                >::type
            >::type
        >::type;

    std::size_t line;
    std::size_t col;

    value_type value;
};

struct program{
    std::vector<ast_node> function_declarations;
    static const char symbol[];
};

struct decl_func{
    std::string function_name;
    boost::optional<ast_node> maybe_declaration_params;
    ast_node statement;
    static const char symbol[];
};

struct decl_params{
    std::vector<ast_node> declaration_params;
    static const char symbol[];
};

struct list_match{
    std::vector<std::string> elements;
    std::string rest_elems_name;
    static const char symbol[];
};

struct type_match{
    std::string param_name;
    std::string type_name;
    static const char symbol[];
};

struct statement{
    ast_node value;
    static const char symbol[];
};

struct let_statement{
    std::vector<ast_node> function_declarations;
    static const char symbol[];
};

struct if_statement{
    ast_node condition;
    ast_node expression_if_true;
    ast_node expression_if_false;
    static const char symbol[];
};

struct case_statement{
    std::vector<ast_node> case_when;
    ast_node otherwise_expression;
    static const char symbol[];
};

struct case_when{
    ast_node condition;
    ast_node then_expression;
    static const char symbol[];
};

struct expression{
    std::vector<ast_node> formulae;
    static const char symbol[];
};

struct formula{
    enum struct sign {
        minus = 1,
        plus
    };
    boost::optional<sign> maybe_sign;
    std::vector<ast_node> terms;
    static const char symbol[];
};

struct term{
    std::vector<ast_node> factors;
    static const char symbol[];
};

struct factor{
    ast_node value;
    static const char symbol[];
};

struct relational_operator{
    std::string value;
    static const char symbol[];
};

struct additive_operator{
    std::string value;
    static const char symbol[];
};

struct mult_operator{
    std::string value;
    static const char symbol[];
};

struct constant{
    boost::variant< int
                    , char
                    , bool
                    , std::string
                    , ast_node > value;
    static const char symbol[];
};

struct list{
    ast_node value;
    static const char symbol[];
};

struct enum_list{
    std::vector<ast_node> elements;
    static const char symbol[];
};

struct int_list{
    int min;
    int max;
    static const char symbol[];
};

struct char_list{
    char begin;
    char end;
    static const char symbol[];
};

struct func_call{
    std::string function_name;
    boost::optional<ast_node> maybe_call_arguments;
    static const char symbol[];
};

struct call_args{
    std::vector<ast_node> arguments;
    static const char symbol[];
};

class ast {
public:
    ast_node root;
    bool operator==(ast const& rhs) const;
    bool operator!=(ast const& rhs) const;
};

} // namespace ast
} // namespace templa

#endif    // TEMPLA_AST_HPP_INCLUDED

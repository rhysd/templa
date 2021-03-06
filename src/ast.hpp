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
struct decl_param;
struct list_match;
struct type_match;
struct expression;
struct let_expression;
struct if_expression;
struct case_expression;
struct case_when;
struct primary_expression;
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
                boost::mpl::vector25<
                    program,
                    decl_func,
                    decl_params,
                    decl_param,
                    list_match,
                    type_match,
                    expression,
                    let_expression,
                    if_expression,
                    case_expression,
                    case_when,
                    primary_expression,
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

    value_type value;

    std::size_t line;
    std::size_t col;

};

struct program{
    std::vector<ast_node> function_declarations;
    static const char symbol[];
};

struct decl_func{
    std::string function_name;
    boost::optional<ast_node> maybe_declaration_params;
    ast_node expression;
    static const char symbol[];
};

struct decl_params{
    std::vector<ast_node> declaration_params;
    static const char symbol[];
};

struct decl_param{
    boost::variant<ast_node, std::string> value;
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

struct expression{
    ast_node value;
    static const char symbol[];
};

struct let_expression{
    std::vector<ast_node> function_declarations;
    ast_node body;
    static const char symbol[];
};

struct if_expression{
    ast_node condition;
    ast_node expression_if_true;
    ast_node expression_if_false;
    static const char symbol[];
};

struct case_expression{
    std::vector<ast_node> case_when;
    ast_node otherwise_expression;
    static const char symbol[];
};

struct case_when{
    ast_node condition;
    ast_node then_expression;
    static const char symbol[];
};

struct primary_expression{
    std::vector<ast_node> formulae;
    std::vector<ast_node> operators;
    static const char symbol[];
};

struct formula{
    boost::optional<char> maybe_sign;
    std::vector<ast_node> terms;
    std::vector<ast_node> operators;
    static const char symbol[];
};

struct term{
    std::vector<ast_node> factors;
    std::vector<ast_node> operators;
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

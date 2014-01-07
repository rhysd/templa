#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/range/algorithm.hpp>

#include "ast.hpp"

namespace templa {
namespace ast {

char const program::symbol[] = "PROGRAM";
char const decl_func::symbol[] = "DECL_FUNC";
char const decl_params::symbol[] = "DECL_PARAMS";
char const list_match::symbol[] = "LIST_MATCH";
char const type_match::symbol[] = "TYPE_MATCH";
char const statement::symbol[] = "STMT";
char const let_statement::symbol[] = "LET_STMT";
char const if_statement::symbol[] = "IF_STMT";
char const case_statement::symbol[] = "CASE_STMT";
char const case_when::symbol[] = "CASE_WHEN";
char const expression::symbol[] = "EXPR";
char const formula::symbol[] = "FORM";
char const term::symbol[] = "TERM";
char const factor::symbol[] = "FACT";
char const relational_operator::symbol[] = "RELATIONAL_OP";
char const additive_operator::symbol[] = "ADDITIVE_OP";
char const mult_operator::symbol[] = "MULT_OP";
char const constant::symbol[] = "CONSTANT";
char const list::symbol[] = "LIST";
char const enum_list::symbol[] = "ENUM_LIST";
char const int_list::symbol[] = "INT_LIST";
char const char_list::symbol[] = "CHAR_LIST";
char const func_call::symbol[] = "FUNC_CALL";
char const call_args::symbol[] = "CALL_ARGS";

struct equality_checker : boost::static_visitor<bool> {

    bool operator()(program const& lhs, program const& rhs) const
    {
        return boost::equal(lhs.function_declarations, rhs.function_declarations);
    }

    bool operator()(decl_func const& lhs, decl_func const& rhs) const
    {
        return lhs.function_name == rhs.function_name
                && lhs.maybe_declaration_params == rhs.maybe_declaration_params
                && lhs.statement == rhs.statement;
    }

    bool operator()(decl_params const& lhs, decl_params const& rhs) const
    {
        return boost::equal(lhs.declaration_params, rhs.declaration_params);
    }

    bool operator()(list_match const& lhs, list_match const& rhs) const
    {
        return boost::equal(lhs.elements, rhs.elements) &&
                lhs.rest_elems_name == rhs.rest_elems_name;
    }

    bool operator()(type_match const& lhs, type_match const& rhs) const
    {
        return lhs.param_name == rhs.param_name && lhs.type_name == rhs.type_name;
    }

    bool operator()(statement const& lhs, statement const& rhs) const
    {
        return lhs.value ==  rhs.value;
    }

    bool operator()(let_statement const& lhs, let_statement const& rhs) const
    {
        return boost::equal(lhs.function_declarations, rhs.function_declarations);
    }

    bool operator()(if_statement const& lhs, if_statement const& rhs) const
    {
        return lhs.condition == rhs.condition &&
               lhs.expression_if_true == rhs.expression_if_true &&
               lhs.expression_if_false == rhs.expression_if_false;
    }

    bool operator()(case_statement const& lhs, case_statement const& rhs) const
    {
        return boost::equal(lhs.case_when, rhs.case_when) &&
                lhs.otherwise_expression == rhs.otherwise_expression;
    }

    bool operator()(case_when const& lhs, case_when const& rhs) const
    {
        return lhs.condition == rhs.condition
                && lhs.then_expression == rhs.then_expression;
    }

    bool operator()(expression const& lhs, expression const& rhs) const
    {
        return boost::equal(lhs.formulae, rhs.formulae);
    }

    bool operator()(formula const& lhs, formula const& rhs) const
    {
        return lhs.maybe_sign == rhs.maybe_sign && boost::equal(lhs.terms, rhs.terms);
    }

    bool operator()(term const& lhs, term const& rhs) const
    {
        return boost::equal(lhs.factors, rhs.factors);
    }

    bool operator()(factor const& lhs, factor const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(relational_operator const& lhs, relational_operator const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(additive_operator const& lhs, additive_operator const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(mult_operator const& lhs, mult_operator const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(constant const& lhs, constant const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(list const& lhs, list const& rhs) const
    {
        return lhs.value == rhs.value;
    }

    bool operator()(enum_list const& lhs, enum_list const& rhs) const
    {
        return boost::equal(lhs.elements, rhs.elements);
    }

    bool operator()(int_list const& lhs, int_list const& rhs) const
    {
        return lhs.min == rhs.min && lhs.max == rhs.max;
    }

    bool operator()(char_list const& lhs, char_list const& rhs) const
    {
        return lhs.begin == rhs.begin && lhs.end == rhs.end;
    }

    bool operator()(func_call const& lhs, func_call const& rhs) const
    {
        return lhs.function_name == rhs.function_name &&
                lhs.maybe_call_arguments == rhs.maybe_call_arguments;
    }

    bool operator()(call_args const& lhs, call_args const& rhs) const
    {
        return boost::equal(lhs.arguments, rhs.arguments);
    }

    // Otherwise
    template<class T, class U>
    bool operator()(T const&, U const&) const
    {
        return false;
    }
};

bool ast_node::operator==(ast_node const& rhs) const
{
    return boost::apply_visitor(equality_checker{}, this->value, rhs.value);
}

bool ast_node::operator!=(ast_node const& rhs) const
{
    return !(*this == rhs);
}

bool ast::operator==(ast const& rhs) const
{
    return boost::apply_visitor(equality_checker{}, this->root.value, rhs.root.value);
}

bool ast::operator!=(ast const& rhs) const
{
    return !(*this == rhs);
}

} // namespace ast
} // namespace templa

#include <cstddef>

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

#include "ast_dumper.hpp"
#include "helper/variant.hpp"

namespace templa {
namespace ast {

struct ast_dumper : boost::static_visitor<std::string> {

    ast_dumper(std::size_t const indent)
        : indent(indent)
    {}

    std::string operator()(program const& ast) const;
    std::string operator()(decl_func const& ast) const;
    std::string operator()(decl_params const& ast) const;
    std::string operator()(list_match const& ast) const;
    std::string operator()(type_match const& ast) const;
    std::string operator()(statement const& ast) const;
    std::string operator()(let_statement const& ast) const;
    std::string operator()(if_statement const& ast) const;
    std::string operator()(case_statement const& ast) const;
    std::string operator()(case_when const& ast) const;
    std::string operator()(expression const& ast) const;
    std::string operator()(formula const& ast) const;
    std::string operator()(term const& ast) const;
    std::string operator()(factor const& ast) const;
    std::string operator()(relational_operator const& ast) const;
    std::string operator()(additive_operator const& ast) const;
    std::string operator()(mult_operator const& ast) const;
    std::string operator()(constant const& ast) const;
    std::string operator()(list const& ast) const;
    std::string operator()(enum_list const& ast) const;
    std::string operator()(int_list const& ast) const;
    std::string operator()(char_list const& ast) const;
    std::string operator()(func_call const& ast) const;
    std::string operator()(call_args const& ast) const;

private:
    std::string visit_ast(ast_node const& ast) const;
    template<class A>
    std::string symbol(A const &) const;

    std::size_t indent;
};

using boost::adaptors::transformed;
using boost::algorithm::join;

struct constant_visitor : boost::static_visitor<std::string> {
    constant_visitor(std::size_t const indent)
        : indent(indent)
    {}

    std::string operator()(ast_node const& a) const
    {
        return boost::apply_visitor(ast_dumper{indent + 1}, a.value);
    }

    template<class T>
    std::string operator()(T const& t) const
    {
        return std::string(indent+1, ' ') + boost::lexical_cast<std::string>(t);
    }
    std::size_t indent;
};

std::string ast_dumper::visit_ast(ast_node const& ast) const
{
    return boost::apply_visitor(ast_dumper{indent + 1}, ast.value);
}

template<class A>
std::string ast_dumper::symbol(A const &) const
{
    return std::string(indent, ' ') + A::symbol + "\n";
}

std::string ast_dumper::operator()(program const& ast) const
{
    return symbol(ast) + join(
               ast.function_declarations | transformed([this](auto const& a){
                       return visit_ast(a);
                   })
               , "\n"
           );
}

std::string ast_dumper::operator()(decl_func const& ast) const
{
    auto result = symbol(ast) + std::string(indent+1, ' ') + "FUNC_NAME: " + ast.function_name + "\n";
    if (ast.maybe_declaration_params) {
        result += visit_ast(*ast.maybe_declaration_params) + "\n";
    }
    result += visit_ast(ast.statement);
    return result;
}

std::string ast_dumper::operator()(decl_params const& ast) const 
{
    return symbol(ast) + join(
                ast.declaration_params | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string ast_dumper::operator()(list_match const& ast) const
{
    return symbol(ast) + join(
                ast.elements | transformed([this](auto const& s){ return "DECL_PARAM: " + s; })
              , "\n"
            );
}

std::string ast_dumper::operator()(type_match const& ast) const
{
    return symbol(ast) +
        std::string(indent+1, ' ') + "PARAM_NAME: " + ast.param_name + "\n" +
        std::string(indent+1, ' ') + "TYPE_NAME: " + ast.type_name + "\n";
}

std::string ast_dumper::operator()(statement const& ast) const
{
    return symbol(ast) + visit_ast(ast.value);
}

std::string ast_dumper::operator()(let_statement const& ast) const
{
    return symbol(ast) + join(
                ast.function_declarations | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string ast_dumper::operator()(if_statement const& ast) const
{
    return symbol(ast) +
            visit_ast(ast.condition) + "\n" +
            visit_ast(ast.expression_if_true) + "\n" +
            visit_ast(ast.expression_if_false);
}

std::string ast_dumper::operator()(case_statement const& ast) const
{
    return symbol(ast) + join(
                ast.case_when | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            ) + "\n" + visit_ast(ast.otherwise_expression);
}

std::string ast_dumper::operator()(case_when const& ast) const
{
    return symbol(ast) + visit_ast(ast.condition) + "\n" + visit_ast(ast.then_expression);
}

std::string ast_dumper::operator()(expression const& ast) const
{
    return symbol(ast) + join(
                ast.formulae | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string ast_dumper::operator()(formula const& ast) const
{
    auto result = symbol(ast);
    if (ast.maybe_sign) {
        auto &sign = *ast.maybe_sign;
        result += std::string(indent+1, ' ') + "SIGN: " + (sign == formula::sign::minus ? "-" : "+") + "\n";
    }
    result += join(
                ast.terms | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
    return result;
}

std::string ast_dumper::operator()(term const& ast) const
{
    return symbol(ast) + join(
                ast.factors | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string ast_dumper::operator()(factor const& ast) const
{
    return symbol(ast) + visit_ast(ast.value);
}

std::string ast_dumper::operator()(relational_operator const& ast) const
{
    return std::string(indent, ' ') + relational_operator::symbol + ": " + ast.value;
}

std::string ast_dumper::operator()(additive_operator const& ast) const
{
    return std::string(indent, ' ') + additive_operator::symbol + ": " + ast.value;
}

std::string ast_dumper::operator()(mult_operator const& ast) const
{
    return std::string(indent, ' ') + mult_operator::symbol + ": " + ast.value;
}

std::string ast_dumper::operator()(constant const& ast) const
{
    return symbol(ast) + boost::apply_visitor(constant_visitor{indent}, ast.value);
}

std::string ast_dumper::operator()(list const& ast) const
{
    return symbol(ast) + visit_ast(ast.value);
}

std::string ast_dumper::operator()(enum_list const& ast) const
{
    return symbol(ast) + join(
                ast.elements | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string ast_dumper::operator()(int_list const& ast) const
{
    return symbol(ast) +
        std::string(indent+1, ' ') + "LIST_MIN: " + std::to_string(ast.min) + "\n" +
        std::string(indent+1, ' ') + "LIST_MAX: " + std::to_string(ast.max);
}

std::string ast_dumper::operator()(char_list const& ast) const
{
    return symbol(ast) +
        std::string(indent+1, ' ') + "CHAR_BEGIN: " + std::to_string(ast.begin) + "\n" +
        std::string(indent+1, ' ') + "CHAR_END: " + std::to_string(ast.end);
}

std::string ast_dumper::operator()(func_call const& ast) const
{
    auto result = symbol(ast) + std::string(indent+1, ' ') + "FUNC_NAME: " + ast.function_name;
    if (ast.maybe_call_arguments) {
        result += "\n" + visit_ast(*ast.maybe_call_arguments);
    }
    return result;
}

std::string ast_dumper::operator()(call_args const& ast) const
{
    return symbol(ast) + join(
                ast.arguments | transformed([this](auto const& a){ return visit_ast(a); })
              , "\n"
            );
}

std::string dump_ast(ast const& a)
{
    return boost::apply_visitor(ast_dumper{0}, a.root.value);
}

} // namespace ast
} // namespace templa

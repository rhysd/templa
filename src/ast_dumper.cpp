#include <cstddef>

#define BOOST_RESULT_OF_USE_DECLTYPE 1

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

#include "ast_dumper.hpp"
#include "helper/variant.hpp"

namespace templa {
namespace ast {

using boost::adaptors::transformed;
using boost::algorithm::join;

struct ast_dumper : boost::static_visitor<std::string> {

    ast_dumper(std::size_t const indent)
        : indent(indent)
    {}

    std::string operator()(program const& node) const;
    std::string operator()(decl_func const& node) const;
    std::string operator()(decl_params const& node) const;
    std::string operator()(decl_param const& node) const;
    std::string operator()(list_match const& node) const;
    std::string operator()(type_match const& node) const;
    std::string operator()(expression const& node) const;
    std::string operator()(let_expression const& node) const;
    std::string operator()(if_expression const& node) const;
    std::string operator()(case_expression const& node) const;
    std::string operator()(case_when const& node) const;
    std::string operator()(primary_expression const& node) const;
    std::string operator()(formula const& node) const;
    std::string operator()(term const& node) const;
    std::string operator()(factor const& node) const;
    std::string operator()(relational_operator const& node) const;
    std::string operator()(additive_operator const& node) const;
    std::string operator()(mult_operator const& node) const;
    std::string operator()(constant const& node) const;
    std::string operator()(list const& node) const;
    std::string operator()(enum_list const& node) const;
    std::string operator()(int_list const& node) const;
    std::string operator()(char_list const& node) const;
    std::string operator()(func_call const& node) const;
    std::string operator()(call_args const& node) const;

private:
    std::string visit_node(ast_node const& node) const
    {
        return boost::apply_visitor(ast_dumper{indent + 1}, node.value);
    }

    // Deduct A from its argument
    template<class A>
    std::string symbol(A const &) const
    {
        return std::string(indent, ' ') + A::symbol + "\n";
    }

    std::size_t indent;
};

// Do not be an inner class because template isn't available in inner classes
struct constant_visitor : boost::static_visitor<std::string> {
    constant_visitor(std::size_t const indent)
        : indent(indent)
    {}

    std::string operator()(ast_node const& n) const
    {
        return boost::apply_visitor(ast_dumper{indent + 1}, n.value);
    }

    template<class T>
    std::string operator()(T const& t) const
    {
        return std::string(indent+1, ' ') + boost::lexical_cast<std::string>(t);
    }
    std::size_t indent;
};

std::string ast_dumper::operator()(program const& node) const
{
    return symbol(node) + join(
               node.function_declarations | transformed([this](auto const& n){
                       return visit_node(n);
                   })
               , "\n"
           );
}

std::string ast_dumper::operator()(decl_func const& node) const
{
    auto result = symbol(node) + std::string(indent+1, ' ') + "FUNC_NAME: " + node.function_name + "\n";
    if (node.maybe_declaration_params) {
        result += visit_node(*node.maybe_declaration_params) + "\n";
    }
    result += visit_node(node.expression);
    return result;
}

std::string ast_dumper::operator()(decl_params const& node) const 
{
    return symbol(node) + join(
                node.declaration_params | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
}

std::string ast_dumper::operator()(decl_param const& node) const 
{
    if (auto maybe_match = templa::variant::get<ast_node>(node.value)) {
        return symbol(node) + visit_node(*maybe_match);
    } else {
        return std::string(indent, ' ') + decl_param::symbol + ": " + boost::get<std::string>(node.value) + '\n';
    }
}

std::string ast_dumper::operator()(list_match const& node) const
{
    return symbol(node) + join(
                node.elements | transformed([this](auto const& s){ return std::string(indent+1, ' ') + "ELEM_NAME: " + s; })
              , "\n"
            ) + '\n' + std::string(indent+1, ' ') + "REST_ELEMS_NAME: " + node.rest_elems_name;
}

std::string ast_dumper::operator()(type_match const& node) const
{
    return symbol(node) +
        std::string(indent+1, ' ') + "PARAM_NAME: " + node.param_name + "\n" +
        std::string(indent+1, ' ') + "TYPE_NAME: " + node.type_name + "\n";
}

std::string ast_dumper::operator()(expression const& node) const
{
    return symbol(node) + visit_node(node.value);
}

std::string ast_dumper::operator()(let_expression const& node) const
{
    return symbol(node) + join(
                node.function_declarations | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            ) + "\n"
            + visit_node(node.body);
}

std::string ast_dumper::operator()(if_expression const& node) const
{
    return symbol(node) +
            visit_node(node.condition) + "\n" +
            visit_node(node.expression_if_true) + "\n" +
            visit_node(node.expression_if_false);
}

std::string ast_dumper::operator()(case_expression const& node) const
{
    return symbol(node) + join(
                node.case_when | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            ) + "\n" + visit_node(node.otherwise_expression);
}

std::string ast_dumper::operator()(case_when const& node) const
{
    return symbol(node) + visit_node(node.condition) + "\n" + visit_node(node.then_expression);
}

std::string ast_dumper::operator()(primary_expression const& node) const
{
    return symbol(node) + join(
                node.formulae | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
}

std::string ast_dumper::operator()(formula const& node) const
{
    auto result = symbol(node);
    if (node.maybe_sign) {
        auto &sign = *node.maybe_sign;
        result += std::string(indent+1, ' ') + "SIGN: " + (sign == formula::sign::minus ? "-" : "+") + "\n";
    }
    result += join(
                node.terms | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
    return result;
}

std::string ast_dumper::operator()(term const& node) const
{
    return symbol(node) + join(
                node.factors | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
}

std::string ast_dumper::operator()(factor const& node) const
{
    return symbol(node) + visit_node(node.value);
}

std::string ast_dumper::operator()(relational_operator const& node) const
{
    return std::string(indent, ' ') + relational_operator::symbol + ": " + node.value;
}

std::string ast_dumper::operator()(additive_operator const& node) const
{
    return std::string(indent, ' ') + additive_operator::symbol + ": " + node.value;
}

std::string ast_dumper::operator()(mult_operator const& node) const
{
    return std::string(indent, ' ') + mult_operator::symbol + ": " + node.value;
}

std::string ast_dumper::operator()(constant const& node) const
{
    return symbol(node) + boost::apply_visitor(constant_visitor{indent}, node.value);
}

std::string ast_dumper::operator()(list const& node) const
{
    return symbol(node) + visit_node(node.value);
}

std::string ast_dumper::operator()(enum_list const& node) const
{
    return symbol(node) + join(
                node.elements | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
}

std::string ast_dumper::operator()(int_list const& node) const
{
    return symbol(node) +
        std::string(indent+1, ' ') + "LIST_MIN: " + std::to_string(node.min) + "\n" +
        std::string(indent+1, ' ') + "LIST_MAX: " + std::to_string(node.max);
}

std::string ast_dumper::operator()(char_list const& node) const
{
    return symbol(node) +
        std::string(indent+1, ' ') + "CHAR_BEGIN: " + std::to_string(node.begin) + "\n" +
        std::string(indent+1, ' ') + "CHAR_END: " + std::to_string(node.end);
}

std::string ast_dumper::operator()(func_call const& node) const
{
    auto result = symbol(node) + std::string(indent+1, ' ') + "FUNC_NAME: " + node.function_name;
    if (node.maybe_call_arguments) {
        result += "\n" + visit_node(*node.maybe_call_arguments);
    }
    return result;
}

std::string ast_dumper::operator()(call_args const& node) const
{
    return symbol(node) + join(
                node.arguments | transformed([this](auto const& n){ return visit_node(n); })
              , "\n"
            );
}

std::string dump_ast(ast const& a)
{
    return boost::apply_visitor(ast_dumper{0}, a.root.value);
}

} // namespace ast
} // namespace templa

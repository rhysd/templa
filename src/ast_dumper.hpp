#if !defined TEMPLA_AST_DUMPER_HPP_INCLUDED
#define      TEMPLA_AST_DUMPER_HPP_INCLUDED

#include <string>

#include "ast.hpp"

namespace templa {
namespace ast {

std::string dump_ast(ast const& a);

} // namespace ast
} // namespace templa

#endif    // TEMPLA_AST_DUMPER_HPP_INCLUDED

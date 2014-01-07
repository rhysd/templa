#include "compiler.hpp"
#include "ast_dumper.hpp"

#include <iostream>

namespace templa {

std::string compiler::compile(std::string const& code)
{
    ast::ast a = parser.parse(code);

    // TODO: Temporary
    std::cout << ast::dump_ast(a) << std::endl;
    return "";
}

} // namespace templa

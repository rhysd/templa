#include "compiler.hpp"

namespace templa {

std::string compiler::compile(std::string const& code)
{
    parser.parse(code);
    return "";
}

} // namespace templa

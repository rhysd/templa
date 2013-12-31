#include "compiler.hpp"

namespace templa {

void compiler::compile(std::string const& code)
{
    parser.parse(code);
}

} // namespace templa

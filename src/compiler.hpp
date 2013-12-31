#if !defined TEMPLA_COMPILER_HPP_INCLUDED
#define      TEMPLA_COMPILER_HPP_INCLUDED

#include <string>

#include "parser.hpp"

namespace templa {

class compiler{
public:
    void compile(std::string const& code);
private:
    syntax::parser parser;
};

} // namespace templa

#endif    // TEMPLA_COMPILER_HPP_INCLUDED

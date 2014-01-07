#if !defined TEMPLA_PARSER_HPP_INCLUDED
#define      TEMPLA_PARSER_HPP_INCLUDED

#include <string>

#include "ast.hpp"

namespace templa {
namespace syntax {

class parser{
public:
    ast::ast parse(std::string const& code);
};

} // namespace syntax
} // namespace templa
#endif    // TEMPLA_PARSER_HPP_INCLUDED

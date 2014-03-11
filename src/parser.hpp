#if !defined TEMPLA_PARSER_HPP_INCLUDED
#define      TEMPLA_PARSER_HPP_INCLUDED

#include <string>
#include <stdexcept>
#include <cstddef>
#include <sstream>

#include "ast.hpp"

namespace templa {
namespace syntax {

using std::size_t;

class parser{
public:
    ast::ast parse(std::string const& code);
};

class parse_error : public std::runtime_error {
    static std::ostringstream buffer;
    size_t line, col;
public:
    parse_error(size_t const line, size_t const col)
        : std::runtime_error(""), line(line), col(col)
    {}

    virtual char const* what() const noexcept;
};

} // namespace syntax
} // namespace templa
#endif    // TEMPLA_PARSER_HPP_INCLUDED

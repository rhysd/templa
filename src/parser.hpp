#if !defined TEMPLA_PARSER_HPP_INCLUDED
#define      TEMPLA_PARSER_HPP_INCLUDED

#include <string>

namespace templa {
namespace syntax {

class parser{
public:
    void parse(std::string const& code);
};

} // namespace syntax
} // namespace templa
#endif    // TEMPLA_PARSER_HPP_INCLUDED

#if !defined TEMPLA_PHOENIX_HELPER_HPP_INCLUDED
#define      TEMPLA_PHOENIX_HELPER_HPP_INCLUDED

#include <utility>

#include <boost/spirit/include/phoenix_function.hpp>

#include "ast.hpp"

namespace templa {
namespace phoenix {
namespace detail {

    template<class Node>
    struct make_ast_node_impl {
        template<class... Args>
        Node operator()( Args &&... args ) const
        {
            return { std::forward<Args>(args)... };
        }
    };

} // namespace detail

template<class Node, class... Args>
inline auto make_ast_node(Args &&... args)
{
    return boost::phoenix::function<detail::make_ast_node_impl<Node>>{}( std::forward<Args>(args)... );
}

} // namespace phoenix
} // namespace templa

#endif    // TEMPLA_PHOENIX_HELPER_HPP_INCLUDED

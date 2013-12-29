#include <boost/mpl/vector.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/char.hpp>
#include <boost/mpl/string.hpp>

using namespace boost;

typedef mpl::vector<mpl::int_<1>, mpl::char_<'a'>, mpl::string<'h', 'o', 'g', 'e'>> hoge;

typedef mpl::range_c<int, 1, 10> hoge2;

template<class X>
struct two_elem_of;

template<class X, class Y, class... XS>
struct two_elem_of<mpl::vector<X, Y, XS...>> {
    using x = X;
    using y = Y;
    using xs = mpl::vector<XS...>;
    using type = mpl::vector<x, y>;
};

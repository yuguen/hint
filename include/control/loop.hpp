#ifndef LOOP_HPP
#define LOOP_HPP

#include <cstddef>
#include <type_traits>

using namespace std;


template<size_t first, size_t end, size_t inc, typename T>
class static_loop_imp{};

template<size_t init, size_t end, size_t inc>
using static_loop = static_loop_imp<init, end, inc, integral_constant<bool, (init < end)> >;

//Inspired from https://stackoverflow.com/a/45819050/5159937
template<size_t first, size_t end, size_t inc>
class static_loop_imp<first, end, inc, true_type>
{
    template<typename Lambda>
    static inline constexpr void
    apply(Lambda const & func)
    {
        if (first < end) {
            func(integral_constant<int, first>{});
            static_loop<first+inc, end, inc>::apply(func);
        }
    }
};

template<size_t first, size_t end, size_t inc>
class static_loop_imp<first, end, inc, false_type>
{
    template<typename Lambda>
    static inline constexpr void apply(Lambda const &)
    {
    }
};



#endif // LOOP_HPP

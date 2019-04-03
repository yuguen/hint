#pragma once
#include <cstddef>
#include <type_traits>
#include <iostream>

using namespace std;

/***
 * CRTP Base class
 */
template <size_t W, bool is_signed, template<size_t, bool> class wrapper>
class hint_base{
public:
    using type =  hint_base<W, is_signed, wrapper >;
    using wrapper_type = wrapper<W, is_signed>;
    template<size_t S, bool sign>
    using wrapper_helper = wrapper<S, sign>;

    template<size_t high, size_t low>
    wrapper_helper<high - low + 1, false> slice(
        typename enable_if<high >= low>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->do_slicing(high, low);
    }

    template<size_t idx>
    wrapper_helper<1, false> get(
       typename enable_if<idx <= W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->do_get(idx);
    }


    template<size_t Wrhs, bool isSignedRhs>
    wrapper_helper<W + Wrhs, is_signed>
    concatenate(wrapper_helper<Wrhs, isSignedRhs> const & val) const
    {
        return static_cast<wrapper_type const *>(this)->do_concatenate(val);
    }

    void print(ostream & out) const
    {
        static_cast<wrapper_type const *>(this)->to_stream(out);
    }

    friend ostream & operator<<(ostream & out, type const & val)
    {
        val.print(out);
        return out;
    }
};

#if defined(BITSET_BACKEND)
#include "backend/bitset_impl.ipp"
#endif

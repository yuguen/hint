#pragma once
#include <cstddef>
#include <type_traits>
//#include <iostream>

using namespace std;


/***
 * CRTP Base class
 */
template <size_t W, bool is_signed, template<size_t, bool> class wrapper>
class hint_base{
public:
    using type =  hint_base<W, is_signed, wrapper >;
    typedef  wrapper<W, is_signed> wrapper_type;

    template<size_t high, size_t low>
    wrapper<high - low + 1, false> slice(
        typename enable_if<high >= low and high < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_slicing<high, low>();
    }

    template<size_t idx>
    wrapper<1, false> get(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_get<idx>();
    }

    template<size_t idx>
    bool isSet(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_isset<idx>();
    }

    operator wrapper_type()
    {
        return *reinterpret_cast<wrapper_type*>(this);
    }


    template<size_t Wrhs, bool isSignedRhs>
    wrapper<W + Wrhs, is_signed>
    concatenate(wrapper<Wrhs, isSignedRhs> const & val) const
    {
        return static_cast<wrapper_type const *>(this)->do_concatenate(val);
    }

    wrapper<1, false> operator==(wrapper<W, is_signed> const & rhs) const {
        return static_cast<wrapper_type const *>(this)->compare(rhs);
    }

    template <size_t idx>
    inline constexpr wrapper<1, false> operator[](integral_constant<size_t, idx>) const {
        return get<idx>();
    }

    static inline wrapper<W, false> generateSequence(wrapper<1, false> const & val)
    {
        return wrapper<W, false>::do_generateSequence(val);
    }
};


#if defined(BITSET_BACKEND)
#include "backend/bitset_impl.ipp"
#endif

#if defined(VIVADO_BACKEND)
#include "backend/vivado_impl.ipp"
#endif

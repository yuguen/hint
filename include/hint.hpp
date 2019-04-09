#pragma once
#include <cstddef>
#include <type_traits>
//#include <iostream>

using namespace std;


/***
 * CRTP Base class
 */
template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
class hint_base{
public:
    using type =  hint_base<W, is_signed, wrapper >;
    typedef  wrapper<W, is_signed> wrapper_type;

    template<unsigned int high, unsigned int low>
    wrapper<high - low + 1, false> slice(
        typename enable_if<high >= low and high < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_slicing<high, low>();
    }

    template<unsigned int idx>
    wrapper<1, false> get(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_get<idx>();
    }

    template<unsigned int idx>
    bool isSet(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return static_cast<wrapper_type const *>(this)->template do_isset<idx>();
    }

    operator wrapper_type&()
    {
        return *reinterpret_cast<wrapper_type*>(this);
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    wrapper<W + Wrhs, false>
    concatenate(wrapper<Wrhs, isSignedRhs> const & val) const
    {
        return static_cast<wrapper_type const *>(this)->do_concatenate(val);
    }

    wrapper<1, false> operator==(wrapper<W, is_signed> const & rhs) const {
        return static_cast<wrapper_type const *>(this)->compare(rhs);
    }

    wrapper<W+1, is_signed> addWithCarry(wrapper<W, is_signed> const & op2, wrapper<1, false> const & cin) const
    {
        return static_cast<wrapper<W, is_signed> const *>(this)->perform_addc(
            op2, cin
        );
    }


    wrapper<W, is_signed>& operator=(hint_base<W, is_signed, wrapper> const& rhs)
    {
        auto p = static_cast<wrapper<W, is_signed>*>(this);
        p->do_affect(*static_cast<wrapper<W, is_signed> const *>(&rhs));
        return *p;
    }

    template<bool newSign>
    wrapper<W, newSign> reinterpret_sign(typename enable_if<newSign == is_signed>::type* = 0) const
    {
        return *reinterpret_cast<wrapper<W, is_signed> const *>(this);
    }

    template<bool newSign>
    wrapper<W, newSign> reinterpret_sign(typename enable_if<newSign != is_signed>::type* = 0) const
    {
        return static_cast<wrapper<W, is_signed>const *>(this)->invert_sign();
    }

    wrapper<W, is_signed> modularAdd(wrapper<W, is_signed> const & op2) const
    {
        wrapper<1, false> cin{0};
        wrapper<W+1, is_signed> res = addWithCarry(op2, cin);
        wrapper<W, false> sliced{res.template do_slicing<W-1, 0>()}; 
        return sliced.template reinterpret_sign<is_signed>();
    }

    static wrapper<W, false> generateSequence(wrapper<1, false> const & val)
    {
        return wrapper<W, false>::do_generateSequence(val);
    }

    static wrapper<W, is_signed> mux(
            wrapper<1, false> const & control,
            wrapper<W, is_signed> const & opt1,
            wrapper<W, is_signed> const & opt0
        )
    {
        return wrapper<W, is_signed>::do_mux(control, opt1, opt0);
    }

    wrapper<1, false> or_reduce()
    {
        auto p = static_cast<wrapper<W, is_signed>*>(this);
        return p->do_or_reduce();
    }

};


#if defined(BITSET_BACKEND)
#include "backend/bitset_impl.ipp"
#endif

#if defined(VIVADO_BACKEND)
#include "backend/vivado_impl.ipp"
#endif

#if defined(INTEL_BACKEND)
#include "backend/intel_impl.ipp"
#endif

#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include <type_traits>

#include "ap_int.h"

template<unsigned int W, bool is_signed>
struct VivadoBaseType{};

template<unsigned int W>
struct VivadoBaseType<W, true>
{
    typedef ap_int<W> type;
};

template<unsigned int W>
struct VivadoBaseType<W, false>
{
    typedef ap_uint<W> type;
};

template <unsigned int W, bool is_signed>
class VivadoWrapper;

template <unsigned int W, bool is_signed>
using vivado_hint = hint_base<W, is_signed, VivadoWrapper>;


template <unsigned int W, bool is_signed>
class VivadoWrapper : public vivado_hint<W, is_signed>, private VivadoBaseType<W, is_signed>::type
{
public:
    typedef VivadoWrapper<W, true> type;
    typedef typename VivadoBaseType<W, is_signed>::type storage_type;
    template<unsigned int N>
    using storage_helper = typename VivadoBaseType<N, is_signed>::type;
    template<unsigned int N>
    using us_storage_helper = typename VivadoBaseType<N, false>::type;
    template<unsigned int N>
    using wrapper_helper = VivadoWrapper<N, is_signed>;
    template<unsigned int N>
    using us_wrapper_helper = VivadoWrapper<N, false>;
    typedef  vivado_hint<W, is_signed> hint_type;

    VivadoWrapper():storage_type{0}{}


    VivadoWrapper(storage_type const & val):storage_type{val}{
    }

    template<unsigned int high, unsigned int low>
    VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        return us_wrapper_helper<high - low + 1>{us_storage_helper<high-low+1>{storage_type::range(high, low)}};
    }

    template<unsigned int idx>
    VivadoWrapper<1, false> do_get() const
    {
        return us_wrapper_helper<1>{
            us_storage_helper<1>{
            storage_helper<1>{
                    storage_type::operator[](idx)
            }}
        };
    }

    template<unsigned int idx>
    VivadoWrapper<1, false> get() const
    {
        return hint_type::template get<idx>();
    }

    template<unsigned int idx>
    bool do_isset() const
    {
        return (storage_type::operator[](idx) == 1);
    }

    us_wrapper_helper<W> do_and(wrapper_helper<W> const & rhs) const
    {
        us_storage_helper<W> val {static_cast<storage_type const &>(*this)};
        us_storage_helper<W> rhs_us{static_cast<storage_type const &>(rhs)};
        return us_wrapper_helper<W>{
            val.And(rhs_us)
        };
    }

    us_wrapper_helper<W> do_or(wrapper_helper<W> const & rhs) const
    {
        us_storage_helper<W> val {static_cast<storage_type const &>(*this)};
        us_storage_helper<W> rhs_us{static_cast<storage_type const &>(rhs)};
        return us_wrapper_helper<W>{
            val.Or(rhs_us)
        };
    }

    template<unsigned int newSize>
    us_wrapper_helper<newSize> do_leftpad() const
    {
        return us_wrapper_helper<newSize>{
            us_storage_helper<newSize>{
                us_storage_helper<W>{
                    static_cast<storage_type const &>(*this)
                }
            }
        };
    }



    template<unsigned int Wrhs, bool isSignedRhs>
    VivadoWrapper<Wrhs + W, false> do_concatenate(
            VivadoWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        us_wrapper_helper<Wrhs + W> ret {storage_type::concat(val)};
        return  ret;
    }

    VivadoWrapper<1, false> compare(VivadoWrapper<W, is_signed> const & rhs)const
    {
        return us_wrapper_helper<1>{static_cast<us_storage_helper<1> >(storage_type::operator==(rhs))};
    }

    void do_affect(wrapper_helper<W> const & val)
    {
        storage_type::operator=(val);
    }

    template<bool newSign>
    VivadoWrapper<W, newSign> do_reinterpret_sign() const {
        typename VivadoBaseType<W, newSign>::type val{*this};
        return VivadoWrapper<W, newSign>{val};
    }

    static VivadoWrapper<W, false> do_generateSequence(
            VivadoWrapper<1, false> const & val
        )
    {
        ap_int<1> sign {static_cast<storage_type const &>(val)};
        ap_int<W> ext {sign};
        return VivadoWrapper<W, false>{
            ap_uint<W>{ext}
        };
    }

    wrapper_helper<W+1> perform_addc(
            wrapper_helper<W> const & op2,
            us_wrapper_helper<1> const & cin
        ) const
    {
        return wrapper_helper<W+1>{
            static_cast<storage_helper<W+1> >(*this + op2 + cin)
        };
    }

    VivadoWrapper<1, false> operator==(VivadoWrapper<W, is_signed> const & rhs) const {
        return hint_type::operator==(rhs);
    }


    static wrapper_helper<W> do_mux(
            us_wrapper_helper<1> const & control,
            wrapper_helper<W> const & opt1,
            wrapper_helper<W> const & opt0
        )
    {
        storage_helper<W> res;
        if(control) {
            res = opt1;
        } else {
            res = opt0;
        }
        return wrapper_helper<W>{res};
    }

    us_wrapper_helper<W> convert_unsigned() const
    {
        us_storage_helper<W> usval{reinterpret_cast<storage_type const &>(*this)};
        return us_wrapper_helper<W>{usval};
    }

    us_wrapper_helper<1> do_or_reduce()
    {
        return us_wrapper_helper<1>{storage_type::or_reduce()};
    }

    us_wrapper_helper<1> do_and_reduce()
    {
        return us_wrapper_helper<1>{storage_type::and_reduce()};
    }

    storage_type const & unravel() const
    {
        return static_cast<storage_type const &>(*this);;
    }


    template<unsigned int N, bool val>
    friend class VivadoWrapper;
};


#endif

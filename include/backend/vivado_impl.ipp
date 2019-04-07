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
class VivadoWrapper : public hint_base<W, is_signed, VivadoWrapper>
{
private:
    typename VivadoBaseType<W, is_signed>::type _storage;
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

    VivadoWrapper(storage_type const & val):_storage{val}{
    }

    template<unsigned int high, unsigned int low>
    inline VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        #pragma HLS INLINE
        return us_wrapper_helper<high - low + 1>{us_storage_helper<high-low+1>{_storage.range(high, low)}};
    }

    template<unsigned int idx>
    inline VivadoWrapper<1, false> do_get() const
    {
        #pragma HLS INLINE
        return us_wrapper_helper<1>{
            us_storage_helper<1>{
            storage_helper<1>{
                    _storage[idx]
            }}
        };
    }

    template<unsigned int idx>
    inline bool do_isset() const
    {
        return (_storage[idx] == 1);
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    inline VivadoWrapper<Wrhs + W, false> do_concatenate(
            VivadoWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        #pragma HLS INLINE
        us_storage_helper<Wrhs + W> ret{_storage.concat(val._storage)};
        return  us_wrapper_helper<Wrhs + W>{ret};
    }

    VivadoWrapper<1, false> compare(VivadoWrapper<W, is_signed> const & rhs)const
    {
        return us_wrapper_helper<1>{static_cast<us_storage_helper<1> >(_storage == rhs._storage)};
    }

    inline void do_affect(wrapper_helper<W> const & val)
    {
        _storage = val._storage;
    }

    inline VivadoWrapper<W, not is_signed> invert_sign() const
    {
        typename VivadoBaseType<W, not is_signed>::type val{_storage};
        return VivadoWrapper<W, not is_signed>{val};
    }

    static inline VivadoWrapper<W, false> do_generateSequence(
            VivadoWrapper<1, false> const & val
        )
    {
        ap_int<1> sign = val._storage[0];
        ap_int<W> ext = sign;
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
            static_cast<storage_helper<W+1> >(_storage + op2._storage + cin._storage)
        };
    }

    static inline wrapper_helper<W> do_mux(
            us_wrapper_helper<1> const & control,
            wrapper_helper<W> const & opt1,
            wrapper_helper<W> const & opt0
        )
    {
        storage_helper<W> res;
        if(control._storage) {
            res = opt1._storage;
        } else {
            res = opt0._storage;
        }
        return wrapper_helper<W>{res};
    }

    template<unsigned int N, bool val>
    friend class VivadoWrapper;
};


#endif

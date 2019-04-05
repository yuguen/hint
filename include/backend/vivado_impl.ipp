#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include <type_traits>

#include "ap_int.h"

template <size_t W, bool is_signed>
class VivadoWrapper{};

template<size_t W>
class VivadoWrapper<W, false> : public hint_base<W, false, VivadoWrapper>
{
private:
    ap_uint<static_cast<int>(W)> _storage;
public:
    typedef VivadoWrapper<W, true> type;
    typedef ap_uint<static_cast<int>(W)> storage_type;
    template<size_t N>
    using storage_helper = ap_uint<static_cast<int>(N)>;
    template<size_t N>
    using wrapper_type = VivadoWrapper<N, false>;

    VivadoWrapper(storage_type const & val):_storage{val}{
    }

    VivadoWrapper(int val):_storage{val}{
    }

    template<size_t high, size_t low>
    inline VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        #pragma HLS INLINE
        return VivadoWrapper<high - low + 1, false>{storage_helper<high-low+1>{_storage.range(high, low)}};
    }

    template<size_t idx>
    inline VivadoWrapper<1, false> do_get() const
    {
        #pragma HLS INLINE
        return wrapper_type<1>{
            static_cast<storage_helper<1> >(
                    _storage[idx]
            )
        };
    }

    template<size_t idx>
    inline bool do_isset() const
    {
        return (_storage[idx] == 1);
    }

    template<size_t Wrhs, bool isSignedRhs>
    inline VivadoWrapper<Wrhs + W, false> do_concatenate(
            VivadoWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        #pragma HLS INLINE
        ap_uint<Wrhs + W> ret{_storage->concat(val)};
        return  wrapper_type<Wrhs + W>{ret};
    }

    VivadoWrapper<1, false> compare(VivadoWrapper<W, false> const & rhs)const
    {
        return static_cast<ap_uint<1> >(_storage == rhs._storage);
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

    template<size_t N, bool val>
    friend class VivadoWrapper;
};


#endif

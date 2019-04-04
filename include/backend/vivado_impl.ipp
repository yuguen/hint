#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include "ap_int.h"

template <size_t W, bool is_signed>
class VivadoWrapper{};

template<size_t W>
class VivadoWrapper<W, false> : public hint_base<W, false, VivadoWrapper>, private ap_uint<static_cast<int>(W)>
{
public:
    typedef VivadoWrapper<W, true> type;
    typedef ap_uint<static_cast<int>(W)> underlying_type;
    template<size_t N>
    using storage_type = ap_uint<static_cast<int>(N)>;
    template<size_t N>
    using wrapper_type = VivadoWrapper<W, false>;

    VivadoWrapper(underlying_type const & val):underlying_type{val}{}

    //explicit VivadoWrapper(hint_base<W, false, VivadoWrapper> & value):VivadoWrapper(reinterpret_cast<>())
    //{}

    template<size_t high, size_t low>
    inline VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        #pragma HLS INLINE
        return storage_type<high-low+1>{this->range(high, low)};
    }

    template<size_t idx>
    inline VivadoWrapper<1, false> do_get() const
    {
        #pragma HLS INLINE
        return static_cast<wrapper_type<1>>((*this)[idx]);
    }

    template<size_t idx>
    inline bool do_isset() const
    {
        return (static_cast<ap_uint<1> >((*this)[idx]) == 1);
    }

    template<size_t Wrhs, bool isSignedRhs>
    inline VivadoWrapper<Wrhs + W, false> do_concatenate(
            VivadoWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        #pragma HLS INLINE
        ap_uint<Wrhs + W> ret{this->concat(val)};
        return  ret;
    }

//    void to_stream(ostream & out) const
//    {
//        for (size_t i = 1 ; i <= W ; ++i) {
//            out << static_cast<unsigned int>(_storage[W-i]);
//        }
//    }
};


#endif

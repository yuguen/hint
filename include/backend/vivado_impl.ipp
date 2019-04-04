#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include "ap_int.h"

template <size_t W, bool is_signed>
class VivadoWrapper : public hint_base<W, is_signed, VivadoWrapper>, private ap_int_base<static_cast<int>(W), is_signed>
{
public:
    using type = VivadoWrapper<W, true>;
    typedef ap_uint<W> underlying_type;

    VivadoWrapper(ap_int_base<static_cast<int>(W), is_signed> const & val):_storage{val}{}

    //explicit VivadoWrapper(hint_base<W, false, VivadoWrapper> & value):VivadoWrapper(reinterpret_cast<>())
    //{}

    template<size_t high, size_t low>
    inline VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        #pragma HLS INLINE
        return static_cast<ap_uint<static_cast<int>(high - low + 1)> >(_storage.range(high, low));
    }

    template<size_t idx>
    inline VivadoWrapper<1, false> do_get() const
    {
        #pragma HLS INLINE
        return static_cast<ap_uint<1> >((*this)[idx]);
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

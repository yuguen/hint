#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include "ap_int.h"

template<size_t W, bool is_signed> class VivadoWrapper;

template <size_t W>
class VivadoWrapper<W, false> : public hint_base<W, false, BitsetWrapper>
{
private:
    ap_uint<static_cast<int>(W)> _storage;
public:
    using type = VivadoWrapper<W, true>;

    VivadoWrapper(ap_uint<static_cast<int>(W)> const & val):_storage{val}{}

    template<size_t high, size_t low>
    VivadoWrapper<high - low + 1, false> do_slicing() const
    {
        return static_cast<ap_uint<static_cast<int>(high - low + 1)> >(_storage.range(high, low));
    }

    template<size_t idx>
    VivadoWrapper<1, false> do_get() const
    {
        return static_cast<ap_uint<1> >(_storage[idx]);
    }

    template<size_t Wrhs, bool isSignedRhs>
    VivadoWrapper<Wrhs + W, true> do_concatenate(
            VivadoWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        ap_uint<Wrhs + W> ret {_storage.concat(val)};
        return  ret;
    }

    void to_stream(ostream & out) const
    {
        for (size_t i = 1 ; i <= W ; ++i) {
            out << static_cast<unsigned int>(_storage[W-i]);
        }
    }

    template<size_t W, bool isSigned> friend class VivadoWrapper;
};


#endif

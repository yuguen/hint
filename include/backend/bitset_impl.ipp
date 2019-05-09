#ifndef BITSET_IMPL_IPP
#define BITSET_IMPL_IPP

#include <bitset>

using namespace std;

template<unsigned int W, bool is_signed>
class BitsetWrapper : public hint_base<W, is_signed, BitsetWrapper>
{
private:
    bitset<W> _storage;

public:
    using type = BitsetWrapper<W, is_signed>;

    BitsetWrapper(bitset<W> const & val) : _storage{val}{}

    template<unsigned int high, unsigned int low>
    BitsetWrapper<high - low + 1, false> do_slicing() const
    {
        bitset<high - low + 1> ret;
        for(unsigned int i = 0 ; i <= high - low ; ++i) {
            ret[i] = _storage[i+low];
        }
        return BitsetWrapper<high - low + 1, false>{ret};
    }

    template<unsigned int idx>
    BitsetWrapper<1, false> do_get() const
    {
        bitset<1> ret;
        ret[0] = _storage[idx];
        return ret;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    BitsetWrapper<Wrhs + W, is_signed> do_concatenate(
            BitsetWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        bitset<Wrhs + W> ret;
        for (unsigned int i = 0 ; i < Wrhs; ++i) {
            ret[i] = val[i];
        }
        for (unsigned int i = 0 ; i < W ; ++i) {
            ret[i+Wrhs] = _storage[i];
        }
        return ret;
    }
};

#endif // BITSET_IMPL_IPP

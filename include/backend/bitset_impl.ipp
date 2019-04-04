#ifndef BITSET_IMPL_IPP
#define BITSET_IMPL_IPP

#include <bitset>

using namespace std;

template<size_t W, bool is_signed>
class BitsetWrapper : public hint_base<W, is_signed, BitsetWrapper>
{
private:
    bitset<W> _storage;

public:
    using type = BitsetWrapper<W, is_signed>;

    BitsetWrapper(bitset<W> const & val) : _storage{val}{}

    template<size_t high, size_t low>
    BitsetWrapper<high - low + 1, false> do_slicing() const
    {
        bitset<high - low + 1> ret;
        for(size_t i = 0 ; i <= high - low ; ++i) {
            ret[i] = _storage[i+low];
        }
        return BitsetWrapper<high - low + 1, false>{ret};
    }

    template<size_t idx>
    BitsetWrapper<1, false> do_get() const
    {
        bitset<1> ret;
        ret[0] = _storage[idx];
        return ret;
    }

    template<size_t Wrhs, bool isSignedRhs>
    BitsetWrapper<Wrhs + W, is_signed> do_concatenate(
            BitsetWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        bitset<Wrhs + W> ret;
        for (size_t i = 0 ; i < Wrhs; ++i) {
            ret[i] = val[i];
        }
        for (size_t i = 0 ; i < W ; ++i) {
            ret[i+Wrhs] = _storage[i];
        }
        return ret;
    }

//    void to_stream(ostream & out) const {
//        for (size_t i = 1 ; i <= W ; ++i) {
//            if (_storage[W-i]) {
//                out << "1";
//            } else {
//                out << "0";
//            }
//        }
//    }

};

#endif // BITSET_IMPL_IPP

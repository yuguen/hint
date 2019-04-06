#ifndef INTEL_IMPL_IPP
#define INTEL_IMPL_IPP

#include <type_traits>

#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"
#endif


template <size_t W, bool is_signed>
class IntelWrapper<W, is_signed> : public hint_base<W, is_signed, IntelWrapper>
{
private:
    ac_int<W, is_signed> _storage;
public:
    typedef IntelWrapper<W, is_signed> type;
    typedef ac_int<W, is_signed> storage_type;
    template<size_t N, bool sign>
    using storage_helper = ac_int<N, sign>;
    template<size_t N>
    using wrapper_helper = IntelWrapper<N, is_signed>;
    template<size_t N>
    using wrapper_type = IntelWrapper<N, is_signed>;


    IntelWrapper(storage_type const & val):_storage{val}{
    }

    // IntelWrapper(int val):_storage{val}{
    // }

    template<size_t high, size_t low>
    inline IntelWrapper<high - low + 1, false> do_slicing() const
    {
        return IntelWrapper<high - low + 1, false>{storage_helper<high-low+1, false>{_storage.template slc<high-low+1>(low)}};

    }

    template<size_t idx>
    inline IntelWrapper<1, false> do_get() const
    {
        return wrapper_type<1>{
            static_cast<storage_helper<1, false> >(
                    _storage.template slc<1>(idx)
            )
        };
    }

    template<size_t idx>
    inline bool do_isset() const
    {
        return (_storage.template slc<1>(idx) == 1);
    }

    template<size_t Wrhs, bool isSignedRhs>
    inline IntelWrapper<Wrhs + W, false> do_concatenate(
            IntelWrapper<Wrhs, isSignedRhs> const & val
        ) const
    {
        ac_int<W+Wrhs, false> concatenation;
        concatenation.template set_slc(0, rhs);
        concatenation.template set_slc(Wrhs, (*this));        
        ac_int<Wrhs + W, false> ret{concatenation};
        return  wrapper_type<Wrhs + W, false>{ret};
    }

    IntelWrapper<1, false> compare(IntelWrapper<W, false> const & rhs)const
    {
        return static_cast<ac_int<1, false> >(_storage == rhs._storage);
    }

    static inline IntelWrapper<W, false> do_generateSequence(
            IntelWrapper<1, false> const & val
            )
    {
        ac_int<2, true> sign = -val;
        ac_int<W> ext = sign;
        return IntelWrapper<W, false>{
            ac_int<W, false>{ext}
        };
    }

    wrapper_helper<W+1> perform_addc(
            wrapper_helper<W> const & op2,
            wrapper_helper<1, false> const & cin
        )
    {
        return wrapper_helper<W+1>{
            static_cast<storage_helper<W+1, is_signed> >(_storage + op2._storage + cin._storage)
        };
    }

    static inline wrapper_helper<W> do_mux(
            wrapper_helper<1, false> const & control,
            wrapper_helper<W> const & opt1,
            wrapper_helper<W> const & opt0
        )
    {
        storage_helper<W, is_signed> res;
        if(control._storage) {
            res = opt0._storage;
        } else {
            res = opt1._storage;
        }
        return wrapper_helper<W>{res};
    }

    template<size_t N, bool val>
    friend class IntelWrapper;
};


#endif

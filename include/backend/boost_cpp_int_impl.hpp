#ifndef BOOST_CPP_INT_IMPL_HPP
#define BOOST_CPP_INT_IMPL_HPP

#include <type_traits>
#include <cstdint>

#include <boost/multiprecision/cpp_int.hpp>

namespace bmp =  boost::multiprecision;

template <unsigned int W, bool is_signed>
class BoostCppIntWrapper : public hint_base<W, is_signed, BoostCppIntWrapper>
{
public:

    using type = BoostCppIntWrapper<W, is_signed>;
    using storage_type = bmp::number<bmp::cpp_int_backend<W, W, bmp::unsigned_magnitude, bmp::unchecked, void> >;

private:
    template<unsigned int N, bool val>
    friend class BoostCppIntWrapper;

    storage_type m_storage;

public:
    template<unsigned int N, bool sign>
    using storage_helper = typename BoostCppIntWrapper<N,sign>::storage_type;

    template<unsigned int N, bool sign>
    using wrapper_helper = BoostCppIntWrapper<N, sign>;

    template<unsigned int N, bool sign>
    using wrapper_type = BoostCppIntWrapper<N, sign>;

    BoostCppIntWrapper(storage_type const & val)
        : m_storage{val}
    {
    }
public:

    explicit BoostCppIntWrapper(uint64_t value)
        : m_storage(value)
    {
        const uint64_t max= W==0 ? 0 : 0xFFFFFFFFFFFFFFFFull >> ((64-W)%64);
        assert(value <= max);
    }

    template<unsigned int high, unsigned int low>
    inline BoostCppIntWrapper<high - low + 1, false> do_slicing() const
    {
        using return_type = BoostCppIntWrapper<high - low + 1, false>;
        return return_type( typename return_type::storage_type( m_storage>>low ) );
    }

    template<unsigned int idx>
    inline BoostCppIntWrapper<1, false> do_get() const
    {
        static_assert(idx<W, "Index out of range.");
        using return_type = BoostCppIntWrapper<1, false>;
        return return_type( typename return_type::storage_type( (m_storage>>idx)&1 ) );
    }

    // TODO: Why have both do_get and do_isset?
    template<unsigned int idx>
    inline bool do_isset() const
    {
        static_assert(idx<W, "Index out of range.");
        return  ((m_storage>>idx)&1) != 0;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    inline BoostCppIntWrapper<Wrhs + W, false> do_concatenate(
            BoostCppIntWrapper<Wrhs, isSignedRhs> const & rhs
        ) const
    {
        using return_type=BoostCppIntWrapper<Wrhs + W, false>;
        using return_type_storage=typename return_type::storage_type;
        return_type_storage left(m_storage);
        return_type_storage right(rhs.m_storage);
        return return_type{ (left<<Wrhs) | right };
    }

    BoostCppIntWrapper<1, false> compare(BoostCppIntWrapper<W, is_signed> const & rhs)const
    {
        return BoostCppIntWrapper<1, false>{
            m_storage==rhs.m_storage ? 1u : 0u
        };
    }

    inline BoostCppIntWrapper<W, !is_signed> invert_sign() const
    {
        return BoostCppIntWrapper<W, !is_signed>{m_storage};
    }


    static inline BoostCppIntWrapper<W, false> do_generateSequence(
            BoostCppIntWrapper<1, false> const & val
            )
    {
        storage_type res{};
        if(val.m_storage){
            res=~res;
        }
        return BoostCppIntWrapper<W, false>( res );
    }

    wrapper_helper<W+1, is_signed> perform_addc(
            wrapper_helper<W, is_signed> const & op2,
            wrapper_helper<1, false> const & cin
        ) const
    {
        using return_type = wrapper_helper<W+1, is_signed>;
        using return_storage = typename return_type::storage_type;
        return return_type(
                return_storage(m_storage)+return_storage(op2.m_storage)+return_storage(cin.m_storage)
                );
    }

    static inline wrapper_helper<W, is_signed> do_mux(
            wrapper_helper<1, false> const & control,
            wrapper_helper<W, is_signed> const & opt1,
            wrapper_helper<W, is_signed> const & opt0
        )
    {
        return control.m_storage ? opt1 : opt0;
    }
};


#endif

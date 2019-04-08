#ifndef MASKED_WORD_IMPL_IPP
#define MASKED_WORD_IMPL_IPP

#include <type_traits>
#include <cstdint>

namespace word_array
{

    namespace detail
    {
        template<unsigned W, bool is_signed>
        struct storage
        {
            static_assert(W<=64, "Currently only up to 64 bits is supported.");
            typedef typename storage<W-1,is_signed>::value_type value_type;
            constexpr static int value_width = storage<W-1,is_signed>::value_width;
        };


        template<>
        struct storage<0,true>
        {
            typedef int8_t value_type;
            constexpr static int value_width=8;
        };

        template<>
        struct storage<9,true>
        {
            typedef int16_t value_type;
            constexpr static int value_width=16;
        };

        template<>
        struct storage<17,true>
        {
            typedef int32_t value_type;
            constexpr static int value_width=32;
        };

        template<>
        struct storage<33,true>
        {
            typedef int64_t value_type;
            constexpr static int value_width=64;
        };


        template<>
        struct storage<0,false>
        {
            typedef uint8_t value_type;
            constexpr static int value_width=8;
        };

        template<>
        struct storage<9,false>
        {
            typedef uint16_t value_type;
            constexpr static int value_width=16;
        };

        template<>
        struct storage<17,false>
        {
            typedef uint32_t value_type;
            constexpr static int value_width=32;
        };

        template<>
        struct storage<33,false>
        {
            typedef uint64_t value_type;
            constexpr static int value_width=64;
        };

    };
};

template <unsigned int W, bool is_signed>
class MaskedWordWrapper : public hint_base<W, is_signed, MaskedWordWrapper>
{
public:
    using type = MaskedWordWrapper<W, is_signed>;

    // We actually used unsigned for the storage backend, regardless of front-end signed-ness
    using storage_type = typename word_array::detail::storage<W,false>::value_type;
    constexpr static int storage_width = word_array::detail::storage<W,false>::value_width;

private:
    template<unsigned int N, bool val>
    friend class MaskedWordWrapper;


    static_assert(W <= storage_width, "Storage type is not wide enough.");

    storage_type m_storage;

    static_assert(W<=64, "TODO: This needs to be changed for larger widths.");
    // Storage type is always unsigned, so this is always logical shift
    constexpr static storage_type MASK = (0xFFFFFFFFFFFFFFFFull) >> (64-W);
    static_assert(std::is_unsigned<storage_type>::value, "Storage expected to be unsigned.");

    // Make sure the value is in range.
    static storage_type clamp(storage_type x)
    {
        return x&MASK;
    }


public:
    template<unsigned int N, bool sign>
    using storage_helper = typename word_array::detail::storage<N,sign>::value_type;

    template<unsigned int N, bool sign>
    using wrapper_helper = MaskedWordWrapper<N, sign>;

    template<unsigned int N, bool sign>
    using wrapper_type = MaskedWordWrapper<N, sign>;

    MaskedWordWrapper(storage_type const & val)
        : m_storage{val}
    {
        assert( val <= MASK );
    }

    // low can only be of type int or unsigned int using ac_int
    // TODO: Why does slicing a signed number produce an unsigned number?
    template<unsigned int high, unsigned int low>
    inline MaskedWordWrapper<high - low + 1, false> do_slicing() const
    {
        using return_type = MaskedWordWrapper<high - low + 1, false>;
        return return_type{ return_type::clamp(m_storage>>low) };
    }

    template<unsigned int idx>
    inline MaskedWordWrapper<1, false> do_get() const
    {
        static_assert(idx<W, "Index out of range.");
        using return_type = MaskedWordWrapper<1, false>;
        return return_type{
            return_type::clamp((m_storage>>idx)&1)
        };
    }

    template<unsigned int idx>
    inline bool do_isset() const
    {
        static_assert(idx<W, "Index out of range.");
        return (m_storage>>idx)&1;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    inline MaskedWordWrapper<Wrhs + W, false> do_concatenate(
            MaskedWordWrapper<Wrhs, isSignedRhs> const & rhs
        ) const
    {
        using return_type = MaskedWordWrapper<Wrhs + W, false>;
        auto left=typename return_type::storage_type(m_storage);
        auto right=typename return_type::storage_type(rhs.m_storage);
        return return_type{  (left<<Wrhs) | right };
    }

    MaskedWordWrapper<1, false> compare(MaskedWordWrapper<W, is_signed> const & rhs)const
    {
        return MaskedWordWrapper<1, false>{ (m_storage == rhs.m_storage) };
    }


    // TODO: To me this seems mis-named. It is reinterpreting bits, but this makes it sound
    // like it is creating logic.
    inline MaskedWordWrapper<W, !is_signed> invert_sign() const
    {
        return MaskedWordWrapper<W, !is_signed>{m_storage};
    }


    // TODO: Not entirely sure what this is doing by the name. Is it generating
    // W replicatiosn of the input bit?
    static inline MaskedWordWrapper<W, false> do_generateSequence(
            MaskedWordWrapper<1, false> const & val
            )
    {
        return MaskedWordWrapper<W, false>( val.m_storage ? MASK : 0 );
    }

    wrapper_helper<W+1, is_signed> perform_addc(
            wrapper_helper<W, is_signed> const & op2,
            wrapper_helper<1, false> const & cin
        ) const
    {
        using return_type = wrapper_helper<W+1, is_signed>;
        using return_storage_type = typename return_type::storage_type;
        auto left=return_storage_type(m_storage);
        auto right=return_storage_type(op2.m_storage);
        auto carry =return_storage_type(cin.m_storage);
        return return_type{
            return_type::clamp(left+right+carry)
        };
    }

    static inline wrapper_helper<W, is_signed> do_mux(
            wrapper_helper<1, false> const & control,
            wrapper_helper<W, is_signed> const & opt1,
            wrapper_helper<W, is_signed> const & opt0
        )
    {
        return wrapper_helper<W, is_signed>{
            control.m_storage ? opt1.m_storage : opt0.m_storage
        };
    }
};


#endif

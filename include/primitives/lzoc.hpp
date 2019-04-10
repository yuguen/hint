#ifndef LZC
#define LZC

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/reverse.hpp"

template<unsigned int S>
struct LZOCStageInfo
{
    static constexpr bool NeedsRecursion = (S>1);
    static constexpr bool IsFinalStage = (S==1);
};

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
//N : Power of 2 of the size of the whole LZOC,
//S power of two of the size of the stage
inline Wrapper<N, false> lzoc_stage(
        Wrapper<(1<<N), is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<LZOCStageInfo<N>::NeedsRecursion>::type* = 0
    )
{
    Wrapper<(1 << (N-1)), false> low = input.template slice<(1 << (N-1)) - 1, 0>();

    auto compare = Wrapper<(1<<(N-1)), false>::generateSequence(leading);
    Wrapper<(1 << (N-1)), false> high = input. template slice<(1 << N) - 1, ((1 << N) >> 1) >();

    Wrapper<1, false> cmp = (compare == high);

    Wrapper<1<<(N-1), false> next_stage_input = Wrapper<1<<(N-1), false>::mux(cmp, low, high);

    auto lower_stage = lzoc_stage<(N-1)>(next_stage_input, leading);
    return cmp.concatenate(lower_stage);
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<N, false> lzoc_stage(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<LZOCStageInfo<N>::IsFinalStage>::type* = 0
    )
{
    return (input.template get<0>() == leading);
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<N, false> lzoc(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading)
{
    return lzoc_stage<N>(input, leading);
}

template<unsigned int N>
struct GenericLZOCStageInfo
{
    static constexpr bool is_a_power_of_2 = ((Static_Val<N>::_2pow) == N);
    static constexpr bool is_one = (N==1);
};

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> generic_lzoc_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<GenericLZOCStageInfo<N>::is_a_power_of_2 and GenericLZOCStageInfo<N>::is_one>::type*  = 0)
{
    return (input.template get<0>() == leading);
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> generic_lzoc_stage(
        Wrapper<N, is_signed>  const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<not(GenericLZOCStageInfo<N>::is_a_power_of_2)>::type* = 0)
{
    static constexpr int log2N = Static_Val<N>::_rlog2-1;
    static constexpr int power_of_2_in_N = 1<<log2N;
    static constexpr int rest_of_N = N-power_of_2_in_N;

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_high = generic_lzoc_stage<power_of_2_in_N>(input.template slice<N-1, N-power_of_2_in_N>(), leading);

    Wrapper<Static_Val<rest_of_N>::_rlog2, false> lzoc_low = generic_lzoc_stage<rest_of_N>(input.template slice<N-power_of_2_in_N-1, 0>(), leading);

    Wrapper<Static_Val<N>::_rlog2, false> log2_N_ones = Wrapper<Static_Val<N>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});

    Wrapper<Static_Val<N>::_rlog2, false> nval{N};

    Wrapper<Static_Val<N>::_rlog2- Static_Val<rest_of_N>::_rlog2, false> lzoc_low_padding_zeros{0};

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_if_low = nval.modularAdd(lzoc_low_padding_zeros.concatenate(lzoc_low));

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_cmp = Wrapper<Static_Val<N>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});


    Wrapper<Static_Val<N>::_rlog2, false> final_lzoc;
    Wrapper<1, false> cmp = lzoc_high == lzoc_cmp;

    if (cmp.template isSet<0>())
        final_lzoc = lzoc_if_low;
    else
        final_lzoc = lzoc_high;

    return final_lzoc;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> generic_lzoc_stage(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        typename std::enable_if<GenericLZOCStageInfo<N>::is_a_power_of_2  and not(GenericLZOCStageInfo<N>::is_one)>::type* = 0)
{
    static constexpr int log2N = Static_Val<N>::_log2;

    Wrapper<Static_Val<N>::_log2, false> lzoc_val = lzoc<log2N, is_signed, Wrapper>(input, leading);
    return Wrapper<1, false>{0}.concatenate(lzoc_val);
}


template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> generic_lzoc(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        typename std::enable_if<not(GenericLZOCStageInfo<N>::is_a_power_of_2)>::type* = 0
        )
{
    Wrapper<Static_Val<N>::_rlog2, false> lzoc_val = generic_lzoc_stage<N>(input, leading);
    return lzoc_val;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> generic_lzoc(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        typename std::enable_if<GenericLZOCStageInfo<N>::is_a_power_of_2>::type* = 0
        )
{
    static constexpr int log2N = Static_Val<N>::_log2;

    Wrapper<log2N, false> lzoc_val =  lzoc<log2N, is_signed, Wrapper>(input, leading);
    return lzoc_val;
}


#endif // LZC

#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"

template<size_t S>
struct LZOCStageInfo
{
    static constexpr bool NeedsRecursion = (S>0);
    static constexpr bool IsFinalStage = (S==0);
};

template<size_t N, size_t S, bool is_signed, template<size_t , bool> class Wrapper>
//N : Power of 2 of the size of the whole LZOC,
//S power of two of the size of the stage
inline Wrapper<S + 1 + (1 << N), false> lzoc_shifter_stage(
        Wrapper<(1<<N), is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<LZOCStageInfo<S>::NeedsRecursion>::type* = 0
    )
{
    #pragma HLS INLINE

    auto padding = Wrapper<(1<<S), false>::generateSequence(fill_bit);
    Wrapper<(1 << N) - (1 << S), false> low = input.template slice<(1 << N) - (1 << S) - 1, 0>();

    auto compare = Wrapper<(1<<S), false>::generateSequence(leading);
    Wrapper<(1 << S), false> high = input. template slice<(1 << N) - 1, (1 << N) - (1 << S)>();

    Wrapper<1, false> cmp = (compare == high);

    Wrapper<1<<N, false> next_stage_input = Wrapper<1<<N, false>::mux(cmp, low.concatenate(padding), input);

    auto lower_stage = lzoc_shifter_stage<N, S-1>(next_stage_input, leading, fill_bit);
    return cmp.concatenate(lower_stage);
}

template<size_t N, int S, bool is_signed, template<size_t , bool> class Wrapper>
inline Wrapper<S + 1 + (1 << N), false> lzoc_shifter_stage(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<LZOCStageInfo<S>::IsFinalStage>::type* = 0
    )
{
    #pragma HLS INLINE
    Wrapper<1, false> cmp = (input.template get<((1<<N) - 1)>() == leading);
    Wrapper<(1<<N) - 1, false> low = input.template slice<(1<<N) - 2, 0>();
    Wrapper<(1<<N), false> res = low.concatenate(fill_bit);
    return Wrapper<1+(1<<N), false>::mux(
                cmp,
                Wrapper<1, false>{1}.concatenate(res),
                Wrapper<1, false>{0}.concatenate(input)
            );
}

template<size_t N, int S, bool is_signed, template<size_t , bool> class Wrapper>
Wrapper<N + (1<<N), false> lzoc_shifter(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0)
{
    #pragma HLS INLINE
    return lzoc_shifter_stage<N, N-1>(input, leading, fill_bit);
}

template<size_t N>
struct GenericLZOCStageInfo
{
    static constexpr bool is_a_power_of_2 = ((Static_Val<N>::_2pow) == N);
    static constexpr bool is_one = (N==1);
};

template<size_t N, int S, bool is_signed, template<size_t , bool> class Wrapper>
Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<S>::is_a_power_of_2 and GenericLZOCStageInfo<S>::is_one>::type*  = 0)
{
    #pragma HLS INLINE
    if (input.template get<N - 1>() == leading) {
        Wrapper<N - 1, false> low = input.template slice<N - 2, 0>();
        Wrapper<N, false> res = low.concatenate(fill_bit);
        return Wrapper<1, false>{1}.concatenate(res);
    } else {
        return Wrapper<1, false>{0}.concatenate(input);
    }
}

template<size_t N, int S, bool is_signed, template<size_t , bool> class Wrapper>
Wrapper<Static_Val<S>::_rlog2 + N, true> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed>  const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<not(GenericLZOCStageInfo<S>::is_a_power_of_2)>::type* = 0)
{
    static constexpr int log2S = Static_Val<S>::_rlog2-1;
    static constexpr int power_of_2_in_S = 1<<log2S;
    static constexpr int rest_of_S = S-power_of_2_in_S;

    Wrapper<Static_Val<S>::_rlog2 + N, false> lzoc_shift = generic_lzoc_shifter_stage<N, power_of_2_in_S>(input, leading, fill_bit);

    Wrapper<Static_Val<S>::_rlog2, false> lzoc = lzoc_shift.template slice<Static_Val<S>::_rlog2 + N-1, N>();
    Wrapper<N, false> shift = lzoc_shift.template slice<N-1, 0>();

    Wrapper<Static_Val<rest_of_S>::_rlog2 + N, false> lzoc_shift_rest = generic_lzoc_shifter_stage<N, rest_of_S>( shift, leading, fill_bit);

    Wrapper<Static_Val<rest_of_S>::_rlog2, false> lzoc_rest = lzoc_shift_rest.template slice<Static_Val<rest_of_S>::_rlog2 + N-1, N>();
    Wrapper<N, false> shift_rest = lzoc_shift_rest.template slice<N-1, 0>();

    Wrapper<N, false> final_shift = (lzoc==Wrapper<Static_Val<S>::_rlog2, false>::generateSequence(Wrapper<1, true>{1})) ? shift_rest : shift;
    //TODO lzoc_rest.size <= _rlog2 : concatener des zéros
    Wrapper<Static_Val<S>::_rlog2 - 1, false> sval{S};
    Wrapper<Static_Val<S>::_rlog2, false> lzoc_if_rest = sval.modular_add(lzoc_rest);

    Wrapper<Static_Val<S>::_rlog2, false> lzoc_cmp = Wrapper<Static_Val<S>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});


    ap_uint<Static_Val<S>::_rlog2> final_lzoc;
    if (lzoc == lzoc_cmp)
        final_lzoc = lzoc_if_rest;
    else
        final_lzoc = lzoc;

    return final_lzoc.concatenate(final_shift);
}

template<size_t N, int S, bool is_signed, template<size_t , bool> class Wrapper>
Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        Wrapper<1, false> fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<S>::is_a_power_of_2  and not(GenericLZOCStageInfo<S>::is_one)>::type* = 0)
{
    #pragma HLS INLINE
    Wrapper<S, false> padding = Wrapper<S, false>::generateSequence(fill_bit);
    Wrapper<N-S, false> low = input.template slice<N - S - 1, 0>();
    Wrapper<S, false> high = input.template slice<N-1, N-S>();
    Wrapper<S, false> high_cmp =  Wrapper<S, false>::generateSequence(leading);

    Wrapper<1, true> cmp = (high == high_cmp);

    ap_uint<N> next_stage_input = (cmp) ? low.concatenate(padding) : input;

    auto lower_stage = generic_lzoc_shifter_stage<N, (S>>1) >(next_stage_input, leading, fill_bit);
    return cmp.concatenate(lower_stage);
}


template<size_t N, bool is_signed, template<size_t , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2 + N, false> generic_lzoc_shifter(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        Wrapper<1, false> fill_bit = 0,
        typename std::enable_if<not(GenericLZOCStageInfo<N>::is_a_power_of_2)>::type* = 0
        )
{
    Wrapper<(Static_Val<N>::_rlog2 + N), true> lzoc_shift =  generic_lzoc_shifter_stage<N, N>(input, leading, fill_bit);
    return lzoc_shift;
}

template<size_t N, bool is_signed, template<size_t , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2 + N, true> generic_lzoc_shifter(
        Wrapper<N, is_signed> input,
        Wrapper<1, true> leading,
        Wrapper<1, true> fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<N>::is_a_power_of_2>::type* = 0
        )
{
    static constexpr int log2N = Static_Val<N>::_log2;

    Wrapper<(log2N + (1<<log2N)), true> lzoc_shift =  lzoc_shifter<log2N>(input, leading, fill_bit);
    return lzoc_shift;
}
#endif // LZOC_SHIFTER_HPP

#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"

template<unsigned int S>
struct LZOCStageInfo
{
    static constexpr bool NeedsRecursion = (S>0);
    static constexpr bool IsFinalStage = (S==0);
};

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
//N : Power of 2 of the size of the whole LZOC,
//S power of two of the size of the stage
inline Wrapper<S + 1 + (1 << N), false> lzoc_shifter_stage(
        Wrapper<(1<<N), is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<LZOCStageInfo<S>::NeedsRecursion>::type* = 0
    )
{
    auto padding = Wrapper<(1<<S), false>::generateSequence(fill_bit);
    Wrapper<(1 << N) - (1 << S), false> low = input.template slice<(1 << N) - (1 << S) - 1, 0>();

    auto compare = Wrapper<(1<<S), false>::generateSequence(leading);
    Wrapper<(1 << S), false> high = input. template slice<(1 << N) - 1, (1 << N) - (1 << S)>();

    Wrapper<1, false> cmp = (compare == high);

    Wrapper<1<<N, false> next_stage_input = Wrapper<1<<N, false>::mux(cmp, low.concatenate(padding), input);

    auto lower_stage = lzoc_shifter_stage<N, S-1>(next_stage_input, leading, fill_bit);
    return cmp.concatenate(lower_stage);
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<S + 1 + (1 << N), false> lzoc_shifter_stage(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<LZOCStageInfo<S>::IsFinalStage>::type* = 0
    )
{
    Wrapper<1, false> cmp = (input.template get<((1<<N) - 1)>() == leading);
    Wrapper<(1<<N) - 1, false> low = input.template slice<(1<<N) - 2, 0>();
    Wrapper<(1<<N), false> res = low.concatenate(fill_bit);
    return Wrapper<1+(1<<N), false>::mux(
                cmp,
                Wrapper<1, false>{1}.concatenate(res),
                Wrapper<1, false>{0}.concatenate(input)
            );
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<N + (1<<N), false> lzoc_shifter(
        Wrapper<1<<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0)
{
    return lzoc_shifter_stage<N, N-1>(input, leading, fill_bit);
}

template<unsigned int N>
struct GenericLZOCStageInfo
{
    static constexpr bool is_a_power_of_2 = ((Static_Val<N>::_2pow) == N);
    static constexpr bool is_one = (N==1);
};

template<unsigned int N, int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<S>::is_a_power_of_2 and GenericLZOCStageInfo<S>::is_one>::type*  = 0)
{
    if ((input.template get<N - 1>() == leading).template isSet<0>()) {
        Wrapper<N - 1, false> low = input.template slice<N - 2, 0>();
        Wrapper<N, false> res = low.concatenate(fill_bit);
        return Wrapper<1, false>{1}.concatenate(res);
    } else {
        return Wrapper<1, false>{0}.concatenate(input);
    }
}

template<unsigned int N, int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
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
    Wrapper<Static_Val<S>::_rlog2, false> log2_S_ones = Wrapper<Static_Val<S>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});
    Wrapper<1, false> cmp = Wrapper<1, false>{lzoc == log2_S_ones};
    bool cmp_bool = cmp.template isSet<0>();
    Wrapper<N, false> final_shift;
    if(cmp_bool){
        final_shift = shift_rest; 
    } 
    else{
        final_shift = shift;
    }
    //TODO lzoc_rest.size <= _rlog2 : concatener des zéros
    Wrapper<Static_Val<S>::_rlog2, false> sval{S};
    Wrapper<Static_Val<S>::_rlog2- Static_Val<rest_of_S>::_rlog2, false> lzoc_rest_padding_zeros{0}; 
    Wrapper<Static_Val<S>::_rlog2, false> lzoc_if_rest = sval.modularAdd(lzoc_rest_padding_zeros.concatenate(lzoc_rest));

    Wrapper<Static_Val<S>::_rlog2, false> lzoc_cmp = Wrapper<Static_Val<S>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});


    Wrapper<Static_Val<S>::_rlog2, false> final_lzoc;
    Wrapper<1, false> cmp2 = lzoc == lzoc_cmp;

    if (cmp2.template isSet<0>())
        final_lzoc = lzoc_if_rest;
    else
        final_lzoc = lzoc;

    return final_lzoc.concatenate(final_shift);
}

template<unsigned int N, int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        Wrapper<1, false> fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<S>::is_a_power_of_2  and not(GenericLZOCStageInfo<S>::is_one)>::type* = 0)
{
    Wrapper<S, false> padding = Wrapper<S, false>::generateSequence(fill_bit);
    Wrapper<N-S, false> low = input.template slice<N - S - 1, 0>();
    Wrapper<S, false> high = input.template slice<N-1, N-S>();
    Wrapper<S, false> high_cmp =  Wrapper<S, false>::generateSequence(leading);

    Wrapper<1, false> cmp = (high == high_cmp);

    Wrapper<N, false> next_stage_input;
    if(cmp.template isSet<0>()){
        next_stage_input = low.concatenate(padding);
    }
    else{
        next_stage_input = input;   
    } 

    auto lower_stage = generic_lzoc_shifter_stage<N, (S>>1) >(next_stage_input, leading, fill_bit);
    return cmp.concatenate(lower_stage);
}


template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2 + N, false> generic_lzoc_shifter(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        Wrapper<1, false> fill_bit = 0,
        typename std::enable_if<not(GenericLZOCStageInfo<N>::is_a_power_of_2)>::type* = 0
        )
{
    Wrapper<(Static_Val<N>::_rlog2 + N), false> lzoc_shift =  generic_lzoc_shifter_stage<N, N>(input, leading, fill_bit);
    return lzoc_shift;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2 + N, false> generic_lzoc_shifter(
        Wrapper<N, is_signed> input,
        Wrapper<1, false> leading,
        Wrapper<1, false> fill_bit = 0,
        typename std::enable_if<GenericLZOCStageInfo<N>::is_a_power_of_2>::type* = 0
        )
{
    static constexpr int log2N = Static_Val<N>::_log2;

    Wrapper<(log2N + (1<<log2N)), false> lzoc_shift =  lzoc_shifter<log2N, log2N, is_signed, Wrapper>(input, leading, fill_bit);
    return Wrapper<1,false>{0}.concatenate(lzoc_shift);
}
#endif // LZOC_SHIFTER_HPP

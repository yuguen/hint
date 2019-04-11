#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>



#include "hint.hpp"
#include "tools/static_math.hpp"
// #include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/reverse.hpp"
#include <iostream>

using namespace std;

// template<int N>
// constexpr bool isOneBelow2Pow()
// {
//     return (((1 << Static_Val<N>::_clog2 ) - 1) == N);
// }

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S+1>::_clog2+N, false> getOneBelow2PowLZOC_shift(
            Wrapper<N, is_signed> const & input,
            Wrapper<1, false> const & leading,
            Wrapper<1, false> const & fill_bit = 0,
            typename enable_if<isOneBelow2Pow<S>() and (S>=2)>::type* = 0
        )
{
    // cerr << "Ge 2 S: " << S << endl;
    // cerr << "input: " << to_string(input) << endl; 
    constexpr int upper_half = (1 <<Static_Val<S>::_flog2);
    // cerr << "upper_half: " << upper_half << endl; 

    auto upper = input.template slice<N-1, N-upper_half>();
    auto comp_seq = Wrapper<upper_half, false>::generateSequence(leading);
    auto comp = (upper == comp_seq);
    auto upper_input = input;
    auto low = input.template slice<N-upper_half-1, 0>().concatenate(Wrapper<upper_half, false>::generateSequence(fill_bit));
    auto next_stage_input = Wrapper<N, false>::mux(comp, low, upper_input);
    // cerr << "next stage input: " << to_string(next_stage_input) << endl; 

    auto ret = comp.concatenate(getOneBelow2PowLZOC_shift<N, upper_half-1>(next_stage_input, leading, fill_bit));
    return ret;

}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S+1>::_clog2+N, false> getOneBelow2PowLZOC_shift(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename enable_if<(S==1)>::type* = 0
    )
{
    // cerr << "Eq 1 S: " << S << endl;
    bool top_is_leading = (input.template get<N-1>() == leading).template isSet<0>();
    Wrapper<Static_Val<S>::_rlog2+N, false> case_is_leading = Wrapper<1, false>{1}.concatenate(input.template slice <N-2, 0>().concatenate(fill_bit));
    Wrapper<Static_Val<S>::_rlog2+N, false> case_not_is_leading = Wrapper<1, false>{0}.concatenate(input);
    // cerr << "input: " << to_string(input) << endl;
    // cerr << "top_is_leading: " << top_is_leading << endl;

    return (top_is_leading) ? case_is_leading : case_not_is_leading;
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S+1>::_clog2 + N, false> LZOC_shift(
            Wrapper<N, is_signed> const & input,
            Wrapper<1, false> const & leading,
            Wrapper<1, false> const & fill_bit = 0,
            typename enable_if<isOneBelow2Pow<S>() and (S > 1)>::type* = 0
        )
{
    // cerr << "Call lzoc shift on size " << N << endl;
    return getOneBelow2PowLZOC_shift<N, S>(input, leading, fill_bit);
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S+1>::_clog2 + N, false> LZOC_shift (
        Wrapper<N, is_signed> const & input,
    Wrapper<1, false> const & leading,
    Wrapper<1, false> const & fill_bit = 0,
    typename enable_if<is2Pow<S>() and (S > 1)>::type* = 0
)
{
    // cerr << "Call lzoc shift (pow2) " << N << " S " << S << endl;
    // cerr << "Call with input " << to_string(input) << endl;

    auto upper = input.template slice<N-1, N-S+1>();
    auto lzoc_shift_up = getOneBelow2PowLZOC_shift<N, S-1>(input, leading, fill_bit);
    auto lzoc_up = lzoc_shift_up.template slice<Static_Val<S>::_clog2+N-1, N>();
    auto shift_up = lzoc_shift_up.template slice<N-1, 0>();

    auto is_full_one = lzoc_shift_up.and_reduce();
    auto last_bit_is_leading = input.template get<N-1>() == leading;
    auto onezeroseq = Wrapper<1, false>{1}.concatenate(
                Wrapper<Static_Val<S>::_clog2, false>::generateSequence(
                    Wrapper<1, false>{0}
                    ));

    auto uncomplete = Wrapper<1, false>{0}.concatenate(lzoc_up);

    bool cond = (is_full_one.And(last_bit_is_leading)).template isSet<0>();
        


    Wrapper<Static_Val<S>::_clog2 + 1, false> lzoc;
    Wrapper<N, false> shift;
    if(cond){
        lzoc = onezeroseq;
        shift = shift_up.template slice<N-2, 0>().concatenate(fill_bit); 
    }
    else{
        lzoc = uncomplete;
        shift = shift_up;
    }


    auto result = lzoc.concatenate(shift);
//    cerr << "Value 2pow lzoc : " << to_string(result) << endl;
    return result;
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S+1>::_clog2 + N, false> LZOC_shift (
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename enable_if<not is2Pow<S>() and not isOneBelow2Pow<S>() and (S > 1)>::type* = 0
)
{

   // cerr << "Call lzoc shift " << N << " S " << S << endl;
   //  cerr << "Call with input " << to_string(input) << endl;
    constexpr int upper_size = (1 << (Static_Val<S>::_flog2)) - 1;
    auto lzoc_shift_up = getOneBelow2PowLZOC_shift<N, upper_size>(input, leading, fill_bit);
    auto lzoc_up = lzoc_shift_up.template slice<N + Static_Val<upper_size>::_clog2 -1,N>();
    auto shift_up = lzoc_shift_up.template slice<N-1,0>();

   // cerr << "lzoc shift up " << to_string(lzoc_shift_up) << endl;
   // cerr << "lzoc up " << to_string(lzoc_up) << endl;
   // cerr << "shift up " << to_string(shift_up) << endl;
    auto is_full_one = lzoc_up.and_reduce();
    auto last_bit_is_leading = lzoc_shift_up.template get<N-1>() == leading;
   // cerr << "last_bit_is_leading " << to_string(last_bit_is_leading) << endl;

    auto msb = is_full_one.And(last_bit_is_leading);

    auto low = input.template slice<N-upper_size - 2, 0>().concatenate(Wrapper<upper_size+1, false>::generateSequence(fill_bit));
    auto lzoc_shift_low = LZOC_shift<N, S-(upper_size+1)>(low, leading, fill_bit);
    auto lzoc_low = lzoc_shift_low.template slice<Static_Val<S-(upper_size+1)>::_clog2 + N -1, N>();
    auto shift_low = lzoc_shift_low.template slice<N-1, 0>();
    auto ext_lowcount = lzoc_low.template leftpad<Static_Val<upper_size>::_clog2, 0>();

   // cerr << "lzoc shift low " << to_string(lzoc_shift_low) << endl;
   // cerr << "lzoc low " << to_string(lzoc_low) << endl;
   // cerr << "shift low " << to_string(shift_low) << endl;

    auto lsb = Wrapper<Static_Val<upper_size>::_clog2, false>::mux(
                msb,
                ext_lowcount,
                lzoc_up
                );

    auto lzoc = msb.concatenate(lsb);
    auto shift = Wrapper<N, false>::mux(
        msb,
        shift_low,
        shift_up
        );
    // cerr << "Value compound lzoc : " << to_string(result) << endl;
    return lzoc.concatenate(shift);
}

// template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
// Wrapper<Static_Val<S>::_clog2 + N, false> LZOC_shift (
//         Wrapper<N, is_signed> const & input,
//         Wrapper<1, false> const & leading,
//         Wrapper<1, false> const & fill_bit = 0,
//         typename enable_if<(S == 2)>::type* = 0
//     )
// {
//     // cerr << "Call lzoc on size one" << endl;
//     auto res =  getAlmost2PowLZC(input, leading);
//     // cerr << "Res : " << res << endl;
//     return res;
// }


/*
#include "hint.hpp"
#include "tools/static_math.hpp"

template<unsigned int S>
struct LZOCShifterStageInfo
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
        typename std::enable_if<LZOCShifterStageInfo<S>::NeedsRecursion>::type* = 0
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
        typename std::enable_if<LZOCShifterStageInfo<S>::IsFinalStage>::type* = 0
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
struct GenericLZOCShifterStageInfo
{
    static constexpr bool is_a_power_of_2 = ((Static_Val<N>::_2pow) == N);
    static constexpr bool is_one = (N==1);
};

template<unsigned int N, int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_rlog2 + N, false> generic_lzoc_shifter_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename std::enable_if<GenericLZOCShifterStageInfo<S>::is_a_power_of_2 and GenericLZOCShifterStageInfo<S>::is_one>::type*  = 0)
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
        typename std::enable_if<not(GenericLZOCShifterStageInfo<S>::is_a_power_of_2)>::type* = 0)
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
        typename std::enable_if<GenericLZOCShifterStageInfo<S>::is_a_power_of_2  and not(GenericLZOCShifterStageInfo<S>::is_one)>::type* = 0)
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
        typename std::enable_if<not(GenericLZOCShifterStageInfo<N>::is_a_power_of_2)>::type* = 0
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
        typename std::enable_if<GenericLZOCShifterStageInfo<N>::is_a_power_of_2>::type* = 0
        )
{
    static constexpr int log2N = Static_Val<N>::_log2;

    Wrapper<(log2N + (1<<log2N)), false> lzoc_shift =  lzoc_shifter<log2N, log2N, is_signed, Wrapper>(input, leading, fill_bit);
    return Wrapper<1,false>{0}.concatenate(lzoc_shift);
}
*/
#endif // LZOC_SHIFTER_HPP

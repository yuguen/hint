#ifndef LZC
#define LZC

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/reverse.hpp"
#include <iostream>

using namespace std;

template<int N>
constexpr bool isOneBelow2Pow()
{
    return (((1 << Static_Val<N>::_clog2 ) - 1) == N);
}

template<int N>
constexpr bool is2Pow()
{
    return (1 << Static_Val<N>::_clog2) == N;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> getAlmost2PowLZC(
            Wrapper<N, is_signed> const & input,
            Wrapper<1, false> const & leading,
            typename enable_if<isOneBelow2Pow<N>() and (N>1)>::type* = 0
        )
{
    constexpr int upper_half = (1 <<Static_Val<N>::_flog2);
    auto upper = input.template slice<N-1, N-upper_half>();
    auto comp_seq = Wrapper<upper_half, false>::generateSequence(leading);
    auto comp = (upper == comp_seq);
    auto upper_input = upper.template slice <upper_half - 1, 1>();
    auto low = input.template slice<upper_half-2, 0>();
    auto next_stage_input = Wrapper<upper_half-1, false>::mux(comp, low, upper_input);
    auto ret = comp.concatenate(getAlmost2PowLZC(next_stage_input, leading));
    return ret;
}

template <bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<1, false> getAlmost2PowLZC(
        Wrapper<1, is_signed> const & input,
        Wrapper<1, false> const & leading
    )
{
    auto us_val = input.template reinterpret_sign<false>();
    return (input == leading);
}


template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> lzoc(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename enable_if<isOneBelow2Pow<N>() and (N > 1)>::type* = 0
    )
{
    return  getAlmost2PowLZC(input, leading);
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2 + 1, false> lzoc (
        Wrapper<N, is_signed> const & input,
    Wrapper<1, false> const & leading,
    typename enable_if<is2Pow<N>() and (N > 1)>::type* = 0
)
{

    cerr << "Call lzoc on 2pow " << endl;
    auto upper = input.template slice<N-1, 1>();
    auto lzocup = getAlmost2PowLZC(upper, leading);
    auto is_full_one = lzocup.and_reduce();
    auto last_bit_ok = input.template get<0>() == leading;
    auto onezeroseq = Wrapper<1, false>{1}.concatenate(
                Wrapper<Static_Val<N>::_clog2, false>::generateSequence(
                    Wrapper<1, false>{0}
                    ));
    auto uncomplete = Wrapper<1, false>{0}.concatenate(lzocup);

    auto result = Wrapper<Static_Val<N>::_clog2+1, false>::mux(
                is_full_one.And(last_bit_ok),
                onezeroseq,
                uncomplete
        );
    cerr << "Value 2pow lzoc : " << to_string(result) << endl;
    return result;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> lzoc (
        Wrapper<N, is_signed> const & input,
    Wrapper<1, false> const & leading,
    typename enable_if<not is2Pow<N>() and not isOneBelow2Pow<N>() and (N > 1)>::type* = 0
)
{

    cerr << "Call Compound lzoc " << N << endl;
    constexpr int upper_size = (1 << Static_Val<N>::_flog2) - 1;
    auto upper = input.template slice<N-1, N-upper_size>();
    auto lzocup = getAlmost2PowLZC(upper, leading);

    auto is_full_one = lzocup.and_reduce();
    auto last_bit_ok = input.template get<N-upper_size-1>() == leading;

    auto msb = is_full_one.And(last_bit_ok);

    auto low = input.template slice<N-upper_size - 2, 0>();
    auto lzoclow = lzoc(low, leading);
    auto ext_lowcount = lzoclow.template leftpad<Static_Val<N>::_flog2, 0>();

    auto lsb = Wrapper<Static_Val<N>::_flog2, false>::mux(
                msb,
                ext_lowcount,
                lzocup
                );

    auto result = msb.concatenate(lsb);
    cerr << "Value compound lzoc : " << to_string(result) << endl;
    return result;
}

template<bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<1, false> lzoc(Wrapper<1, is_signed> const & input,
                       Wrapper<1, false> const & leading
    )
{
    cerr << "Call lzoc on size one" << endl;
    auto res =  getAlmost2PowLZC(input, leading);
    cerr << "Res : " << res << endl;
    return res;
}


/*

template<unsigned int S>
struct LZOCStageInfo
{
    static constexpr bool NeedsRecursion = (S>1);
    static constexpr bool IsFinalStage = (S==1);
};

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
//N : Power of 2 of the size of the whole LZOC,
//S power of two of the size of the stage
inline Wrapper<Static_Val<N>::_rlog2, false> lzoc_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<LZOCStageInfo<N>::NeedsRecursion>::type* = 0
    )
{
    cerr << "Call 2 pow with N = " << N << endl;
    Wrapper<(N>>1), false> low = input.template slice<(N>>1) - 1, 0>();

    auto compare = Wrapper<(N>>1), false>::generateSequence(leading);
    Wrapper<(N >> 1), false> high = input. template slice<N - 1, (N >> 1)>();

    Wrapper<1, false> cmp = (compare == high);

    Wrapper<(N >> 1), false> next_stage_input = Wrapper<(N >> 1), false>::mux(cmp, low, high);

    auto lower_stage = lzoc_stage(next_stage_input, leading);
    auto ret = cmp.concatenate(lower_stage);
    cerr << "Ret : " << to_string(ret) << " (" << N << ")" << endl;
    return ret;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<N>::_rlog2, false> lzoc_stage(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename std::enable_if<LZOCStageInfo<N>::IsFinalStage>::type* = 0
    )
{
    return (input.template get<0>() == leading);
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

    Wrapper<power_of_2_in_N, false> high = input.template slice<N-1, N-power_of_2_in_N>();

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_high = generic_lzoc_stage<power_of_2_in_N>(high, leading);

    Wrapper<Static_Val<rest_of_N>::_rlog2, false> lzoc_low = generic_lzoc_stage<rest_of_N>(input.template slice<N-power_of_2_in_N-1, 0>(), leading);

    Wrapper<Static_Val<N>::_rlog2, false> log2_N_ones = Wrapper<Static_Val<N>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});

    Wrapper<Static_Val<N>::_rlog2, false> nval{N};

    Wrapper<Static_Val<N>::_rlog2- Static_Val<rest_of_N>::_rlog2, false> lzoc_low_padding_zeros{0};

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_if_low = nval.addWithCarry(lzoc_low_padding_zeros.concatenate(lzoc_low), Wrapper<1, false>{1})
                                                            .template slice<Static_Val<N>::_rlog2 - 1, 0>();

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_cmp = Wrapper<Static_Val<N>::_rlog2, false>::generateSequence(Wrapper<1, false>{1});


    Wrapper<Static_Val<N>::_rlog2, false> final_lzoc;
    Wrapper<1, false> cmp = lzoc_high == lzoc_cmp;

    if (cmp.template isSet<0>() and (high.template get<0>() == leading).template isSet<0>())
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

    Wrapper<Static_Val<N>::_rlog2, false> lzoc_val = lzoc_stage(input, leading);
    return lzoc_val;
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
    Wrapper<log2N, false> lzoc_val =  lzoc_stage(input, leading);
    return lzoc_val;
}*/


#endif // LZC

#ifndef LZOC
#define LZOC

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/reverse.hpp"
#include <iostream>

using namespace std;

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> getAlmost2PowLZOC(
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
    auto ret = comp.concatenate(getAlmost2PowLZOC(next_stage_input, leading));
    return ret;
}

template <bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<1, false> getAlmost2PowLZOC(
        Wrapper<1, is_signed> const & input,
        Wrapper<1, false> const & leading
    )
{
    auto us_val = input.as_unsigned();
    return (input == leading);
}


template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> lzoc(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        typename enable_if<isOneBelow2Pow<N>() and (N > 1)>::type* = 0
    )
{
    return  getAlmost2PowLZOC(input, leading);
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2 + 1, false> lzoc (
        Wrapper<N, is_signed> const & input,
    Wrapper<1, false> const & leading,
    typename enable_if<is2Pow<N>() and (N > 1)>::type* = 0
)
{

//    cerr << "Call lzoc on 2pow " << endl;
    auto upper = input.template slice<N-1, 1>();
    auto lzocup = getAlmost2PowLZOC(upper, leading);
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
//    cerr << "Value 2pow lzoc : " << to_string(result) << endl;
    return result;
}

template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<N>::_clog2, false> lzoc (
        Wrapper<N, is_signed> const & input,
    Wrapper<1, false> const & leading,
    typename enable_if<not is2Pow<N>() and not isOneBelow2Pow<N>() and (N > 1)>::type* = 0
)
{

//    cerr << "Call Compound lzoc " << N << endl;
    constexpr int upper_size = (1 << Static_Val<N>::_flog2) - 1;
    auto upper = input.template slice<N-1, N-upper_size>();
    auto lzocup = getAlmost2PowLZOC(upper, leading);

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
    // cerr << "Value compound lzoc : " << to_string(result) << endl;
    return result;
}

template<bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<1, false> lzoc(Wrapper<1, is_signed> const & input,
                       Wrapper<1, false> const & leading
    )
{
    // cerr << "Call lzoc on size one" << endl;
    auto res =  getAlmost2PowLZC(input, leading);
    // cerr << "Res : " << res << endl;
    return res;
}
#endif // LZOC

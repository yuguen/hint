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
            typename enable_if<Static_Val<S>::_isOneBelow2Pow and (S>=2)>::type* = 0
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
            typename enable_if<Static_Val<S>::_isOneBelow2Pow and (S > 1)>::type* = 0
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
    typename enable_if<Static_Val<S>::_is2Pow and (S > 1)>::type* = 0
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
        typename enable_if<(not Static_Val<S>::_is2Pow) and (not Static_Val<S>::_isOneBelow2Pow) and (S > 1)>::type* = 0
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
    auto ext_lowcount = lzoc_low.template leftpad<Static_Val<upper_size>::_clog2>();

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
#endif // LZOC_SHIFTER_HPP

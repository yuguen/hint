#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>



#include "hint.hpp"
#include "tools/static_math.hpp"
// #include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/backwards.hpp"
#include <iostream>

using namespace std;

// template<int N>
// constexpr bool isOneBelow2Pow()
// {
//     return (((1 << Static_Val<N>::_clog2 ) - 1) == N);
// }

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage+N, false> getOneBelow2PowLZOC_shift(
            Wrapper<N, false> const & input,
            Wrapper<1, false> const & leading,
            Wrapper<1, false> const & fill_bit,
            typename enable_if<Static_Val<S>::_isOneBelow2Pow and (S>1)>::type* = 0
        )
{
    // cerr << "Ge 2 S: " << S << endl;
    // cerr << "input: " << to_string(input) << endl; 
    constexpr int upper_half = (1 <<Static_Val<S>::_flog2);
    // cerr << "upper_half: " << upper_half << endl; 

    auto upper = input.template slice<N-1, N-upper_half>();
    auto lower = input.template slice<N-upper_half-1, 0>();

    // auto comp_seq = Wrapper<upper_half, false>::generateSequence(leading);
    // auto comp = (upper == comp_seq);


    auto and_red = upper.and_reduction();
    auto or_red = upper.or_reduction();
    auto comp = Wrapper<1, false>::mux(leading, and_red, Wrapper<1, false>{not(or_red).template isSet<0>()});

    auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);
    // if(fill_bit.template isSet<0>()){
    //     padding = invert(Wrapper<upper_half, false>{0}); 
    // }
    // else{
    //     padding = Wrapper<upper_half, false>{0}; 
    // }

    // auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);

    auto next_stage_input = Wrapper<N, false>::mux(
                comp,
                lower.concatenate(padding),
                input
            );
    // cerr << "next stage input: " << to_string(next_stage_input) << endl; 

    auto ret = comp.concatenate(getOneBelow2PowLZOC_shift<N, upper_half-1>(next_stage_input, leading, fill_bit));
    return ret;
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage +N, false> getOneBelow2PowLZOC_shift(
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit,
        typename enable_if<(S==1) and (N >= 2)>::type* = 0
    )
{
    // cerr << "Eq 1 S: " << S << endl;
    auto top_is_leading = (input.template get<N-1>() == leading);
    auto lower = input.template slice<N-2, 0>();
    auto shifted = Wrapper<N, false>::mux(
                top_is_leading,
                lower.concatenate(fill_bit),
                input
            );
    auto ret = top_is_leading.concatenate(shifted);
    return ret;
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage +N, false> getOneBelow2PowLZOC_shift(
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit,
        typename enable_if<(S==1) and (N == 1)>::type* = 0
    )
{
    // cerr << "Eq 1 S: " << S << endl;
    auto top_is_leading = (input.template get<0>() == leading);
    auto ret = top_is_leading.concatenate(
                Wrapper<1, false>::mux(top_is_leading, fill_bit, input.template get<0>())
            );
    return ret;
}

/*
template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_stage(
            Wrapper<N, false> const & input,
            Wrapper<1, false> const & leading,
            Wrapper<1, false> const & fill_bit = 0,
            typename enable_if<(Static_Val<S>::_isOneBelow2Pow and (S > 1)) or (S==1)>::type* = 0
        )
{
    // cerr << "Call lzoc shift on size " << N << endl;
    return getOneBelow2PowLZOC_shift<N, S>(input, leading, fill_bit);
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_stage (
        Wrapper<N, false> const & input,
    Wrapper<1, false> const & leading,
    Wrapper<1, false> const & fill_bit = 0,
    typename enable_if<Static_Val<S>::_is2Pow and (S > 1)>::type* = 0
)
{
    auto upper = input.template slice<N-1, N-S+1>();
    auto lzoc_shift_up = getOneBelow2PowLZOC_shift<N, S-1>(input, leading, fill_bit);
    auto lzoc_up = lzoc_shift_up.template slice<Static_Val<S>::_clog2+N-1, N>();
    auto shift_up = lzoc_shift_up.template slice<N-1, 0>();

    auto is_full_one = lzoc_shift_up.and_reduction();
    auto last_bit_is_leading = input.template get<N-1>() == leading;
    Wrapper<Static_Val<S>::_clog2, false> zeros{0};
    auto onezeroseq = Wrapper<1, false>{1}.concatenate(zeros);
    auto uncomplete = Wrapper<1, false>{0}.concatenate(lzoc_up);

    auto cond = is_full_one.bitwise_and(last_bit_is_leading);
    auto lzoc = Wrapper<Static_Val<S>::_storage, false>::mux(cond, onezeroseq, uncomplete);
    auto shift = Wrapper<N, false>::mux(
                cond,
                shift_up.template slice<N-2, 0>().concatenate(fill_bit),
                shift_up);

    return lzoc.concatenate(shift);
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_stage (
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0,
        typename enable_if<(not Static_Val<S>::_is2Pow) and (not Static_Val<S>::_isOneBelow2Pow) and (S > 1)>::type* = 0
)
{
    constexpr int upper_size = (1 << (Static_Val<S>::_flog2)) - 1;
    auto lzoc_shift_up = getOneBelow2PowLZOC_shift<N, upper_size>(input, leading, fill_bit);

    auto lzoc_up = lzoc_shift_up.template slice<N + Static_Val<upper_size>::_clog2 -1,N>();
    auto shift_up = lzoc_shift_up.template slice<N-1,0>();
    auto is_full_one = lzoc_up.and_reduction();
    auto last_bit_is_leading = lzoc_shift_up.template get<N-1>() == leading;
    auto msb = is_full_one.bitwise_and(last_bit_is_leading);

    //If all was zero, we only need to count on the lowest bits
    constexpr unsigned int low_shift_size = N - (upper_size+1);
    auto low_lzoc_input = shift_up.template slice<N-2, N-low_shift_size-1>();
    constexpr unsigned int count_size_low = S-(upper_size+1);
    constexpr unsigned int low_lzoc_size = Static_Val<count_size_low>::_storage;
    constexpr unsigned int low_lzoc_total_size = low_lzoc_size + low_shift_size;

    Wrapper<low_lzoc_total_size, false> lzoc_shift_low = LZOC_shift_stage<low_shift_size, count_size_low>(
                low_lzoc_input,
                leading,
                fill_bit
            );
    auto lzoc_low = lzoc_shift_low.template slice<low_lzoc_total_size-1, low_lzoc_total_size-low_lzoc_size>();
    auto shift_low = lzoc_shift_low.template slice<low_shift_size - 1, 0>();
    auto ext_lowcount = lzoc_low.template leftpad<Static_Val<upper_size>::_clog2>();
    auto padding = Wrapper<upper_size+1, false>::generateSequence(fill_bit);

    auto lsb = Wrapper<Static_Val<upper_size>::_clog2, false>::mux(
                msb,
                ext_lowcount,
                lzoc_up
            );

    auto lzoc = msb.concatenate(lsb);
    auto shift = Wrapper<N, false>::mux(
        msb,
        shift_low.concatenate(padding),
        shift_up
        );
    return lzoc.concatenate(shift);
}*/

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit,
        typename enable_if<(N >= S) and (Static_Val<S>::_isOneBelow2Pow or (S==1))>::type* = 0
)
{
    return getOneBelow2PowLZOC_shift<N, S>(input, leading, fill_bit);
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit,
        typename enable_if<(N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and Static_Val<S>::_is2Pow>::type* = 0
)
{
    constexpr unsigned int lzoc_up_size = Static_Val<S-1>::_storage;

    auto lzoc_shifted_up = getOneBelow2PowLZOC_shift<N, S-1>(input.as_unsigned(), leading, fill_bit);

    auto lzoc_up = lzoc_shifted_up.template slice<N+lzoc_up_size - 1, N>();
    auto shifted_up = lzoc_shifted_up.template slice<N-1, 0>();

    auto finalIsLeading = (input.template get<0>() == leading);
    auto allTopIsLeading = lzoc_up.and_reduction();

    auto msb = finalIsLeading.bitwise_and(allTopIsLeading);
    auto lzoc_lsb = Wrapper<lzoc_up_size, false>::mux(
                    msb,
                    Wrapper<lzoc_up_size, false>{0},
                    lzoc_up
                );
    auto lzoc = msb.concatenate(lzoc_lsb);
    auto shifted_final = Wrapper<N, false>::mux(
                msb,
                Wrapper<N, false>::generateSequence(fill_bit),
                shifted_up
            );
    return lzoc.concatenate(shifted_final);
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit,
        typename enable_if<(N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and not(Static_Val<S>::_is2Pow)>::type* = 0
)
{
    constexpr unsigned int count_size = (1 << Static_Val<S>::_storage) - 1;
    constexpr unsigned int lzoc_size = Static_Val<S>::_storage;
    auto lzoc_shifted_up = getOneBelow2PowLZOC_shift<N, count_size>(input, leading, fill_bit);
    auto lzoc_up = lzoc_shifted_up.template slice<N+lzoc_size-1, N>();
    auto shifted = lzoc_shifted_up.template slice<N-1, 0>();

    auto lzoc_has_overflowed = lzoc_up.and_reduction();
    auto lzoc_final = Wrapper<lzoc_size, false>::mux(
                lzoc_has_overflowed,
                Wrapper<lzoc_size, false>{S},
                lzoc_up
                );
    return lzoc_final.concatenate(shifted);
}

template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift(
        Wrapper<N, is_signed> const & input,
        Wrapper<1, false> const & leading,
        Wrapper<1, false> const & fill_bit = 0
)
{
    return LZOC_shift_impl<N, S>(input.as_unsigned(), leading, fill_bit);
}


#endif // LZOC_SHIFTER_HPP

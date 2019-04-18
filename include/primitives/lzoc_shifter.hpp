#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>



#include "hint.hpp"
#include "tools/static_math.hpp"
// #include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/backwards.hpp"
#include "primitives/reductions.hpp"
#include <iostream>

using namespace std;

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_storage+N, false> getOneBelow2PowLZOC_shift(
            Wrapper<N, false> const input,
            Wrapper<1, false> const leading,
            Wrapper<1, false> const fill_bit,
            typename enable_if<Static_Val<S>::_isOneBelow2Pow and (S>1)>::type* = 0
        )
{
    constexpr int upper_half = (1 <<Static_Val<S>::_flog2);
    // cerr << "upper_half: " << upper_half << endl;

    auto upper = input.template slice<N-1, N-upper_half>();
    auto lower = input.template slice<N-upper_half-1, 0>();


    auto and_red = upper.and_reduction();
    auto or_red = or_reduction<16>(upper);
    auto comp = Wrapper<1, false>::mux(leading, and_red, Wrapper<1, false>{not(or_red).template isSet<0>()});

    auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);

    auto next_stage_input = Wrapper<N, false>::mux(
                comp,
                lower.concatenate(padding),
                input
            );

    auto ret = comp.concatenate(getOneBelow2PowLZOC_shift<N, upper_half-1>(next_stage_input, leading, fill_bit));
    return ret;
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_storage +N, false> getOneBelow2PowLZOC_shift(
        Wrapper<N, false> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit,
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
inline Wrapper<Static_Val<S>::_storage +N, false> getOneBelow2PowLZOC_shift(
        Wrapper<N, false> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit,
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


template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit,
        typename enable_if<(N >= S) and (Static_Val<S>::_isOneBelow2Pow or (S==1))>::type* = 0
)
{
    return getOneBelow2PowLZOC_shift<N, S>(input, leading, fill_bit);
}

template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
inline Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit,
        typename enable_if<(N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and Static_Val<S>::_is2Pow>::type* = 0
)
{
    constexpr unsigned int lzoc_up_size = Static_Val<S-1>::_storage;

    auto lzoc_shifted_up = getOneBelow2PowLZOC_shift<N, S-1>(input, leading, fill_bit);

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
inline Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift_impl(
        Wrapper<N, false> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit,
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
inline Wrapper<Static_Val<S>::_storage + N, false> LZOC_shift(
        Wrapper<N, is_signed> const input,
        Wrapper<1, false> const leading,
        Wrapper<1, false> const fill_bit = 0
)
{
    return LZOC_shift_impl<N, S>(input.as_unsigned(), leading, fill_bit);
}


#endif // LZOC_SHIFTER_HPP

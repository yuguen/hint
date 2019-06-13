#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/backwards.hpp"
#include "primitives/reductions.hpp"

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

	// -------- Backward stuff due to intel HLS that sometimes shifts slices that doesn't start at index 0 ---
	auto backwards_input = backwards(input);
	auto upper_backwards = backwards_input.template slice<upper_half-1,0>();
    auto upper = backwards(upper_backwards); 
    // auto upper = input.template slice<N-1, N-upper_half>();
	// ---------------------------------------------------------------------------------------------------------
    auto lower = input.template slice<N-upper_half-1, 0>();

	//cerr << "lower : " << to_string(lower) << endl;


    auto and_red = upper.and_reduction();
	//cerr << "and_red : " << to_string(and_red) << endl;

    auto or_red = upper.or_reduction();
	//cerr << "or_red : " << to_string(or_red) << endl;
    // or_reduction<128>(upper);
	auto op1 = and_red;
	auto op0inv = or_red.template get<0>();
	auto op0 = op0inv.invert();
	auto comp = Wrapper<1, false>::mux(leading, op1, op0);
	auto test = 17;
    auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);

    auto next_stage_input = Wrapper<N, false>::mux(
                comp,
                lower.concatenate(padding),
                input
            );
	//cerr << "Next stage input : " << to_string(next_stage_input) << endl;
	auto intermediate  = getOneBelow2PowLZOC_shift<N, upper_half-1>(next_stage_input, leading, fill_bit);
	auto ret = comp.concatenate(intermediate);
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

    // auto lzoc_up = lzoc_shifted_up.template slice<N+lzoc_up_size - 1, N>();

    auto backwards_lzoc_shifted_up = backwards(lzoc_shifted_up);
    auto lzoc_up_backwards = backwards_lzoc_shifted_up.template slice<lzoc_up_size-1,0>();
    auto lzoc_up = backwards(lzoc_up_backwards); 



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


    // auto lzoc_up = lzoc_shifted_up.template slice<N+lzoc_size-1, N>();
    auto backwards_lzoc_shifted_up = backwards(lzoc_shifted_up);
    auto lzoc_up_backwards = backwards_lzoc_shifted_up.template slice<lzoc_size-1,0>();
    auto lzoc_up = backwards(lzoc_up_backwards); 

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
		Wrapper<1, false> const fill_bit = {0}
)
{
	auto ret = LZOC_shift_impl<N, S>(input.as_unsigned(), leading, fill_bit);
	return ret;
}


#endif // LZOC_SHIFTER_HPP

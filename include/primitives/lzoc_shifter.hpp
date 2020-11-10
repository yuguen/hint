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

using namespace std;

namespace hint {

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	struct lzoc_shift_t
	{
		Wrapper<Static_Val<S>::_storage, false> lzoc;
		Wrapper<N, false> shifted;
	};

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> getOneBelow2PowLZOC_shift(
				Wrapper<N, false> const & input,
				Wrapper<1, false> const & fill_bit,
				typename enable_if<(S>1 and (S < 2*N))>::type* = 0
			)
	{
		constexpr int upper_half = (1 <<Static_Val<S>::_flog2);
		auto upper = input.template slice<N-1, N-upper_half>();
		auto lower = input.template slice<N-upper_half-1, 0>();

		auto comp = (upper == Wrapper<upper_half, false>{0});
		auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);
		auto mux_select = Wrapper<N, false>::generateSequence(comp);
		auto all_zero_input = lower.concatenate(padding) & mux_select;
		auto not_all_zero_input = input & mux_select.invert();

		auto next_stage_input = all_zero_input | not_all_zero_input;

		auto intermediate  = getOneBelow2PowLZOC_shift<N, S-upper_half>(next_stage_input, fill_bit);
		lzoc_shift_t<N, S, Wrapper> ret{comp.concatenate(intermediate.lzoc), intermediate.shifted};
		return ret;
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> getOneBelow2PowLZOC_shift(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(S==1) and (N >= 2)>::type* = 0
		)
	{
		// cerr << "Eq 1 S: " << S << endl;
		auto top_is_leading = (input.template get<N-1>() == Wrapper<1, false>{0});
		auto lower = input.template slice<N-2, 0>();
		auto shifted = Wrapper<N, false>::mux(
					top_is_leading,
					lower.concatenate(fill_bit),
					input
				);
		lzoc_shift_t<N, S, Wrapper> ret{top_is_leading, shifted};
		return ret;
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> getOneBelow2PowLZOC_shift(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(S==1) and (N == 1)>::type* = 0
		)
	{
		// cerr << "Eq 1 S: " << S << endl;
		auto top_is_leading = (input == Wrapper<1, false>{0});
		lzoc_shift_t<N, S, Wrapper> ret{top_is_leading, (top_is_leading &  fill_bit) | (input & top_is_leading.invert())};
		return ret;
	}


	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZOC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(2*N >= S) and (Static_Val<S>::_isOneBelow2Pow or (S==1))>::type* = 0
	)
	{
		return getOneBelow2PowLZOC_shift<N, S, Wrapper>(input, fill_bit);
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZOC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(2*N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and Static_Val<S>::_is2Pow>::type* = 0
	)
	{
		constexpr unsigned int lzoc_up_size = Static_Val<S-1>::_storage;

		auto lzoc_shifted_up = getOneBelow2PowLZOC_shift<N, S-1>(input, fill_bit);

		auto lzoc_up = lzoc_shifted_up.lzoc;
		auto shifted_up = lzoc_shifted_up.shifted;

		auto finalIsLeading = (shifted_up.template get<N-1>() == Wrapper<1, false>{0});
		auto allTopIsLeading = (lzoc_up == Wrapper<lzoc_up_size, false>::generateSequence({{}}));

		auto msb = finalIsLeading & allTopIsLeading;
		auto lzoc_lsb = Wrapper<lzoc_up_size, false>::generateSequence(msb.invert()) & lzoc_up;
		auto lzoc = msb.concatenate(lzoc_lsb);
		auto shifted_final = Wrapper<N, false>::mux(
					msb,
					shifted_up.template slice<N-2,0>().concatenate(fill_bit),
					shifted_up
				);
		return {lzoc, shifted_final};
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZOC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(2*N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and not(Static_Val<S>::_is2Pow)>::type* = 0
	)
	{

		constexpr unsigned int lzoc_size = Static_Val<S>::_storage;
		constexpr unsigned int count_size = (1 << lzoc_size) - 1;
		auto lzoc_shifted_up = getOneBelow2PowLZOC_shift<N, count_size>(input, fill_bit);


		auto lzoc_up = lzoc_shifted_up.lzoc;
		//auto backwards_lzoc_shifted_up = backwards(lzoc_shifted_up);
		//auto lzoc_up_backwards = backwards_lzoc_shifted_up.template slice<lzoc_size-1,0>();
		//auto lzoc_up = backwards(lzoc_up_backwards);

		auto shifted = lzoc_shifted_up.shifted;

		auto lzoc_of_mask = Wrapper<lzoc_size, false>::generateSequence({{1}});

		auto lzoc_has_overflowed = (lzoc_up == lzoc_of_mask);
		auto lzoc_final = Wrapper<lzoc_size, false>::mux(
					lzoc_has_overflowed,
					Wrapper<lzoc_size, false>{S},
					lzoc_up
					);
		return {lzoc_final, shifted};
	}

	/**
	 * N : size of the input
	 * S : how many bit to count and shift (must be a power of 2, or one below a power of two)
	 * is_signed : don't care
	 */
	template<unsigned int N, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZOC_shift(
			Wrapper<N, is_signed> const & input,
			Wrapper<1, false> const & leading,
			Wrapper<1, false> const & fill_bit = {0}
	)
	{
		auto input_us = input.as_unsigned();
		auto invert_mask = Wrapper<N, false>::generateSequence(leading);
		auto real_input = input_us.bitwise_xor(invert_mask);
		auto lzc_shift = LZOC_shift_impl<N, S, Wrapper>(real_input, fill_bit^leading);
		auto final_shifted = invert_mask xor lzc_shift.shifted;
		return lzoc_shift_t<N, S, Wrapper>{lzc_shift.lzoc, final_shifted};
	}
}

#endif // LZOC_SHIFTER_HPP

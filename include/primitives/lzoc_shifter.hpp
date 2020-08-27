#ifndef LZOC_SHIFTER_HPP
#define LZOC_SHIFTER_HPP
//#include <iostream>
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
	inline lzoc_shift_t<N, S, Wrapper> LZC_Shift_req(
				Wrapper<N, false> const & input,
				Wrapper<1, false> const & fill_bit,
				typename enable_if<(S>1)>::type* = 0
			)
	{
		constexpr int upper_half = (1 <<Static_Val<S>::_flog2);
		auto upper = input.template slice<N-1, N-upper_half>();
		auto lower = input.template slice<N-upper_half-1, 0>();

		//cerr << "lower : " << to_string(lower) << endl;

		auto empty = upper.nor_reduction();
		auto padding = Wrapper<upper_half, false>::generateSequence(fill_bit);
		auto all_zero_input = lower.concatenate(padding);
		auto not_all_zero_input = input;

		auto next_stage_input = Wrapper<N, false>::mux(empty, all_zero_input, not_all_zero_input);

		//cerr << "Next stage input : " << to_string(next_stage_input) << endl;
		auto intermediate = LZC_Shift_req<N, (upper_half>>1)>(next_stage_input, fill_bit);
		lzoc_shift_t<N, S, Wrapper> ret{empty.concatenate(intermediate.lzoc), intermediate.shifted};
		return ret;
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZC_Shift_req(
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
	inline lzoc_shift_t<N, S, Wrapper> LZC_Shift_req(
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
	inline lzoc_shift_t<N, S, Wrapper> LZC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(N >= S) and (Static_Val<S>::_isOneBelow2Pow or (S==1))>::type* = 0
	)
	{
		//cerr << "OneBelow2Pow" << endl;
		return LZC_Shift_req<N, S>(input, fill_bit);
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and Static_Val<S>::_is2Pow>::type* = 0
	)
	{
		//cerr << "Twopow" << endl;
		auto lzoc_shifted_up = LZC_Shift_req<N, S-1>(input, fill_bit);
		auto lzoc = lzoc_shifted_up.lzoc;
		auto lzoc_full = lzoc.and_reduction();
		auto last = input.template get<0>();
		auto last_is_zero = last.invert();
		auto should_overflow = lzoc_full.bitwise_and(last_is_zero);
		auto lzoc_fin = lzoc.addWithCarry(should_overflow.template leftpad<Static_Val<S-1>::_storage>(), {0});
		auto shifted = lzoc_shifted_up.shifted;
		auto tail = shifted.template slice <N-2, 0>();
		auto top = shifted.template get<N-1>();
		auto top_fin = top | (lzoc_full & fill_bit);
		auto shifted_fin = top_fin.concatenate(tail);

		return lzoc_shift_t<N, S, Wrapper>{lzoc_fin, shifted_fin};
	}

	template<unsigned int N, unsigned int S, template<unsigned int , bool> class Wrapper>
	inline lzoc_shift_t<N, S, Wrapper> LZC_shift_impl(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & fill_bit,
			typename enable_if<(N >= S) and not(Static_Val<S>::_isOneBelow2Pow or (S==1)) and not(Static_Val<S>::_is2Pow)>::type* = 0
	)
	{
//		cerr << "General" << endl;
		constexpr unsigned int lzoc_size = Static_Val<S>::_storage;
		constexpr unsigned int count_size = (1 << lzoc_size) - 1;
		auto lzoc_shifted_up = LZC_Shift_req<N, count_size>(input, fill_bit);

		auto lzoc_up = lzoc_shifted_up.lzoc;

		auto shifted = lzoc_shifted_up.shifted;

		Wrapper<lzoc_size, false> s_repr {S};

		auto lzoc_has_overflowed = (lzoc_up > s_repr);
		auto lzoc_final = Wrapper<lzoc_size, false>::mux(
					lzoc_has_overflowed,
					s_repr,
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
		auto real_input = Wrapper<N, false>::mux(leading, input_us.invert(), input_us);
		auto lzc_shift = LZC_shift_impl<N, S>(real_input, fill_bit^leading);
		return lzoc_shift_t<N, S, Wrapper>{lzc_shift.lzoc, Wrapper<N, false>::mux(leading, lzc_shift.shifted.invert(), lzc_shift.shifted)};
	}
}

#endif // LZOC_SHIFTER_HPP

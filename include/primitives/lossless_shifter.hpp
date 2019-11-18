#ifndef LOSSLESS_SHIFTER_HPP
#define LOSSLESS_SHIFTER_HPP

#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "primitives/backwards.hpp"

namespace hint {


	template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
	Wrapper<IS+1, false> lossless_shifter_stage(
					Wrapper<IS, is_signed> input,
					Wrapper<S, false> count,
					Wrapper<1, false> sign_bit,
					Wrapper<1, false> fill_bit,
					typename std::enable_if<(S==1)>::type* = 0
			)
	{
			Wrapper<IS+1, false> result;
			if (count.template isSet<0>()) {
					result = input.concatenate(fill_bit);
			} else {
					result = sign_bit.concatenate(input);
			}
			return result;
	}

	template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int, bool> class Wrapper>
	//IS : Input Size,
	//S : size of shift counter
	Wrapper<IS+(1<<S)-1, false> lossless_shifter_stage(
					Wrapper<IS, is_signed> input,
					Wrapper<S, false> count,
					Wrapper<1, false> sign_bit,
					Wrapper<1, false> fill_bit,
					typename std::enable_if<(S>1)>::type* = 0
			)
	{
			Wrapper<(1<<(S-1)), false> padding =  Wrapper<(1<<(S-1)), false>::generateSequence(fill_bit);
			Wrapper<(1<<(S-1)), false> padding_sign =  Wrapper<(1<<(S-1)), false>::generateSequence(sign_bit);
			Wrapper<1, false> stageNeedsShift = count.template get<S-1>();
			Wrapper<S-1, false> countnext = count.template slice<S-2, 0>();

			Wrapper<IS+(1<<(S-1)), false> next_stage_input;
			if (stageNeedsShift.template isSet<0>()) {
					next_stage_input = input.concatenate(padding);
			} else {
					next_stage_input = padding_sign.concatenate(input);
			}
			return lossless_shifter_stage<IS+(1<<(S-1)), S-1, is_signed, Wrapper>(next_stage_input, countnext, sign_bit, fill_bit);
	}




	template< bool isRightShift, unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
	Wrapper<IS+(1<<S)-1, false> lossless_shifter(
					Wrapper<IS, is_signed> input,
					Wrapper<S, false> count,
					Wrapper<1, false> sign_bit = Wrapper<1, false>{0},
					Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
		)
	{
			Wrapper<IS, false> fin_input{isRightShift ? backwards(input) : input};
			Wrapper<IS+(1<<S)-1, false> shift = lossless_shifter_stage<IS, S, is_signed, Wrapper>(fin_input, count, sign_bit, fill_bit);
			Wrapper<IS+(1<<S)-1, false> ret = isRightShift ? backwards (shift) : shift;
			return ret;
	}
}

#endif // LOSSLESS_SHIFTER_HPP

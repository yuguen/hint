#ifndef SHIFTER_STICKY_HPP
#define SHIFTER_STICKY_HPP

#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "backend/config.hpp"
#include "tools/static_math.hpp"

namespace hint {
    template<unsigned int IS, template<unsigned int, bool> class Wrapper>
    struct shifter_sticky_t
    {
          Wrapper<IS, false> shifted;
          Wrapper<1, false> sticky;
    };

    template<unsigned int IS, unsigned int S, unsigned int GroupBy, template<unsigned int, bool> class Wrapper>
    inline shifter_sticky_t<IS, Wrapper> shifter_sticky_stage(
			Wrapper<IS, false> input,
            Wrapper<S, false> count,
			Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
            typename std::enable_if<((IS) >= (1 << (S-1))) and (S > GroupBy)>::type * = 0
		)
	{
        constexpr unsigned int local_shift_max_val = (1 << GroupBy) - 1;
        constexpr unsigned int shiftsize = (local_shift_max_val << (S-GroupBy));
        auto padded_input = Wrapper<shiftsize, false>::generateSequence(fill_bit).concatenate(input.concatenate(Wrapper<shiftsize, false>{0}));
        auto shift_high = count.template slice<S-1, S-GroupBy>();
        auto shift_low = count.template slice<S-GroupBy -1, 0>();
        auto shift = shift_high.concatenate(Wrapper<S - GroupBy, false>{0});
        auto shifted_ext = padded_input >> shift;
        auto shifted = shifted_ext.template slice<IS+shiftsize-1, shiftsize>();
        auto shifted_out = shifted_ext.template slice<shiftsize - 1, 0>();
        auto sticky = (shifted_out == Wrapper<shiftsize, false>{0}).invert();

        auto next_stage = shifter_sticky_stage<IS, S-GroupBy, GroupBy>(shifted, shift_low, fill_bit);
        return {next_stage.shifted, next_stage.sticky | sticky};
	}

    template<unsigned int IS, unsigned int S, unsigned int GroupBy, template<unsigned int, bool> class Wrapper>
    inline shifter_sticky_t<IS, Wrapper> shifter_sticky_stage(
			Wrapper<IS, false> input,
            Wrapper<S, false> count,
            Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
            typename std::enable_if<((IS) >= (1 << (S-1))) and (S <= GroupBy)>::type * = 0
		)
	{
        constexpr unsigned int effectiveGroupBy = (S < GroupBy) ? S : GroupBy;
        constexpr unsigned int local_shift_max_val = (1 << effectiveGroupBy) - 1;
        constexpr unsigned int shiftsize = (local_shift_max_val << (S-effectiveGroupBy));
        auto padded_input = Wrapper<shiftsize, false>::generateSequence(fill_bit).concatenate(input.concatenate(Wrapper<shiftsize, false>{0}));
        auto shifted_ext = padded_input >> count;
        auto shifted = shifted_ext.template slice<IS+shiftsize-1, shiftsize>();
        auto shifted_out = shifted_ext.template slice<shiftsize - 1, 0>();
        auto sticky = (shifted_out == Wrapper<shiftsize, false>{0}).invert();

        return {shifted, sticky};
	}

    template<unsigned int IS, unsigned int S, unsigned int GroupBy, template<unsigned int, bool> class Wrapper>
    inline shifter_sticky_t<IS, Wrapper> shifter_sticky_stage(
			Wrapper<IS, false> input,
            Wrapper<S, false> count,
			Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
			typename std::enable_if<((IS) < (1 << (S-1)))>::type * = 0
		)
	{
		constexpr unsigned int nb_null_shift = S - Static_Val<IS>::_log2;
		auto shift_weights_will_zero = count.template slice<S - 1, S - nb_null_shift>();
		auto next_count = count.template slice<S-nb_null_shift-1, 0>();
        auto stageNeedsShift = (shift_weights_will_zero == Wrapper<nb_null_shift, false>{0}).invert();

        auto sticky = (input == Wrapper<IS, false>{0}).invert();
        auto next_stage = shifter_sticky_stage<IS, S-nb_null_shift, GroupBy, Wrapper>(input, next_count, fill_bit);

        auto ret = Wrapper<IS, false>::mux(
					stageNeedsShift,
                    Wrapper<IS, false>::generateSequence(fill_bit),
                    next_stage.shifted
				);
        auto fin_sticky = (sticky & stageNeedsShift) | next_stage.sticky;
        return {ret, fin_sticky};
	}

    template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper, unsigned int GroupBy = 0>
	inline Wrapper<IS+1, false> shifter_sticky(
					Wrapper<IS, is_signed> input,
					Wrapper<S, false> count,
					Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
		)
	{
        constexpr unsigned int effectiveGroupBy = (GroupBy == 0) ? Config_Values<Wrapper>::shift_group_by : GroupBy;
		auto t = input.as_unsigned();
        auto res = shifter_sticky_stage<IS, S, effectiveGroupBy>(input, count,fill_bit);
        return res.shifted.concatenate(res.sticky);
	}
}

#endif // SHIFTER_STICKY_HPP

#ifndef SHIFTER_STICKY_HPP
#define SHIFTER_STICKY_HPP

#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"

template<unsigned int IS, unsigned int S, template<unsigned int, bool> class Wrapper>
inline Wrapper<IS+1, false> shifter_sticky_stage(
        Wrapper<IS, false> input,
        Wrapper<S, false> count,
        Wrapper<1, false> sticky_in,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
        typename std::enable_if<((IS) >= (1 << (S-1))) and (S > 1)>::type * = 0
    )
{
    constexpr unsigned int shiftsize = (1 << (S-1));

    auto high = input.template slice<IS-1, shiftsize>();
    auto low = input.template slice<shiftsize - 1, 0>();
    auto fill_seq = Wrapper<shiftsize, false>::generateSequence(fill_bit);

    auto needs_shift = count.template get<S-1>();
    auto next_count = count.template slice<S-2, 0>();

    auto next_sticky = Wrapper<1, false>::mux(
                needs_shift,
				sticky_in | low.or_reduction(),
                sticky_in
         );
    auto next_res = Wrapper<IS, false>::mux(
                needs_shift,
                fill_seq.concatenate(high),
                input);

    return shifter_sticky_stage(next_res, next_count, next_sticky, fill_bit);
}

template<unsigned int IS, template<unsigned int, bool> class Wrapper>
inline Wrapper<IS+1, false> shifter_sticky_stage(
        Wrapper<IS, false> input,
        Wrapper<1, false> count,
        Wrapper<1, false> sticky_in,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{

    auto high = input.template slice<IS-1, 1>();
    auto low = input.template get<0>();
	auto needs_shift = count;
    auto final_sticky = Wrapper<1, false>::mux(
                needs_shift,
				sticky_in | low,
                sticky_in
                );
    auto res = Wrapper<IS, false>::mux(
                needs_shift,
                fill_bit.concatenate(high),
                input
              );
    Wrapper<IS+1, false> result = res.concatenate(final_sticky); 
    return result;
}

template<unsigned int IS, unsigned int S, template<unsigned int, bool> class Wrapper>
inline Wrapper<IS+1, false> shifter_sticky_stage(
        Wrapper<IS, false> input,
        Wrapper<S, false> count,
        Wrapper<1, false> sticky_in,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
        typename std::enable_if<((IS) < (1 << (S-1)))>::type * = 0
    )
{
    constexpr unsigned int nb_null_shift = S - Static_Val<IS-1>::_log2;
    auto shift_weights_will_zero = count.template slice<S - 1, S - nb_null_shift>();
    auto next_count = count.template slice<S-nb_null_shift-1, 0>();
    auto stageNeedsShift = shift_weights_will_zero.or_reduction();

    auto sticky = input.or_reduction();

	auto ret = Wrapper<IS+1, false>::mux(
                stageNeedsShift,
				Wrapper<IS, false>::generateSequence(fill_bit).concatenate(sticky),
                shifter_sticky_stage(input, next_count, sticky_in, fill_bit)
            );
    return ret;
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<IS+1, false> shifter_sticky(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
    auto t = input.as_unsigned();
    return shifter_sticky_stage(input, count, Wrapper<1, false>{0}, fill_bit);
}

#endif // SHIFTER_STICKY_HPP

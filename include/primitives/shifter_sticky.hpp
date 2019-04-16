#ifndef SHIFTER_STICKY_HPP
#define SHIFTER_STICKY_HPP

#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "primitives/backwards.hpp"

//template<unsigned int S>
//struct ShifterStickyStageInfo
//{
//    static constexpr bool NeedsRecursion = (S>1);
//    static constexpr bool IsFinalStage = (S==1);
//};

template<unsigned int IS, unsigned int S, template<unsigned int, bool> class Wrapper>
Wrapper<IS+1, false> shifter_sticky_stage(
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
                sticky_in.bitwise_or(low.or_reduction()),
                sticky_in
         );
    auto next_res = Wrapper<IS, false>::mux(
                needs_shift,
                fill_seq.concatenate(high),
                input);

    return shifter_sticky_stage(next_res, next_count, next_sticky, fill_bit);
}

template<unsigned int IS, template<unsigned int, bool> class Wrapper>
Wrapper<IS+1, false> shifter_sticky_stage(
        Wrapper<IS, false> input,
        Wrapper<1, false> count,
        Wrapper<1, false> sticky_in,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
    auto high = input.template slice<IS-1, 1>();
    auto low = input.template get<0>();
    auto& needs_shift = count;
    auto final_sticky = Wrapper<1, false>::mux(
                needs_shift,
                sticky_in.bitwise_or(low),
                sticky_in
                );
    auto res = Wrapper<IS, false>::mux(
                needs_shift,
                fill_bit.concatenate(high),
                input
              );
    return res.concatenate(final_sticky);
}

template<unsigned int IS, unsigned int S, template<unsigned int, bool> class Wrapper>
Wrapper<IS+1, false> shifter_sticky_stage(
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

    auto ret = Wrapper<IS, false>::mux(
                stageNeedsShift,
                sticky.concatenate(Wrapper<IS-1, false>::generateSequence(fill_bit)),
                shifter_sticky_stage(input, next_count, sticky_in, fill_bit)
            );
    return ret;
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<IS+1, false> shifter_sticky(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
    auto t = input.as_unsigned();
    return shifter_sticky_stage(input, count, Wrapper<1, false>{0}, fill_bit);
}

/*
template<unsigned int S>
struct ShifterStickyStageInfo
{
	static constexpr bool NeedsRecursion = (S>1);
	static constexpr bool IsFinalStage = (S==1);
};

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int, bool> class Wrapper>
//IS : Input Size (including sticky bit),
//S : size of shift counter
Wrapper<IS, false> shifter_sticky_stage(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
                typename std::enable_if<ShifterStickyStageInfo<S>::NeedsRecursion>::type* = 0,
                typename std::enable_if<((IS-1) >= (1 << (S-1)))>::type * = 0
        )
{
        auto padding =  Wrapper<1<<(S-1), false>::generateSequence(fill_bit);
        auto stageNeedsShift = count.template get<S-1>();
        auto countnext = count.template slice<S-2, 0>();

        Wrapper<1, false> sticky_in = input.template get<0>();
        Wrapper<IS - 1 - (1 << (S-1)), false> low = input.template slice<IS - 1 - (1 << (S-1)), 1>();
        Wrapper<(1 << (S-1)), false> high = input.template slice<IS - 1 , IS - (1 << (S-1))>();

        Wrapper<IS, false> next_stage_input;
        if (stageNeedsShift.template isSet<0>()) {
                Wrapper<1, false> cur_sticky = Wrapper<1, false>{sticky_in.template isSet<0>() or high.or_reduce().template isSet<0>()};
                next_stage_input = low.concatenate(padding).concatenate(cur_sticky);
        } else {
                next_stage_input = input;
        }
        return shifter_sticky_stage<IS, S-1, is_signed, Wrapper>(next_stage_input, countnext, fill_bit);
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<IS, false> shifter_sticky_stage(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
                typename std::enable_if<ShifterStickyStageInfo<S>::IsFinalStage>::type* = 0,
                typename std::enable_if<((IS-1) >= (1 << (S-1)))>::type * = 0
        )
{
        auto low = input.template slice<IS - 2, 1>();
        auto sticky_out = input.template get<0>().bitwise_or(input.template get<IS-1>());
        auto result = Wrapper<IS, false>::mux(
                    count.template get<0>(),
                    low.concatenate(fill_bit).concatenate(sticky_out),
                    input
                  );
        return result;
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int, bool> class Wrapper>
Wrapper<IS, false> shifter_sticky_stage(
        Wrapper<IS, is_signed> input,
        Wrapper<S, false> count,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
        typename std::enable_if<((IS-1) < (1 << (S-1)))>::type * = 0
    )
{
#pragma warning "Énorme"
    constexpr unsigned int nb_null_shift = S - Static_Val<IS-1>::_log2;
    Wrapper<nb_null_shift, false> shift_weights_will_zero = count.template slice<S - 1, S - nb_null_shift>();
    Wrapper<S-nb_null_shift, false> next_count = count.template slice<S-1-nb_null_shift, 0>();

    Wrapper<1, false> stageNeedsShift = shift_weights_will_zero.or_reduce();
    Wrapper<IS, false> ret;
    if (stageNeedsShift) {
        Wrapper<1, false> sticky = input.or_reduce();
        Wrapper<IS-1, false> high = Wrapper<IS-1, false>::generateSequence(fill_bit);
        ret = high.concatenate(sticky);
    } else {
        ret = shifter_sticky_stage<IS, S-nb_null_shift, is_signed, Wrapper>(input, next_count, fill_bit);
    }
    return ret;
}

template< bool isRightShift, unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<IS+1, false> shifter_sticky(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
        Wrapper<IS, false> fin_input{isRightShift ? reverse(input) : input};
        Wrapper<IS + 1, false> init_sticky = fin_input.concatenate(Wrapper<1, false>{0});
        Wrapper<IS + 1, false> shiftstick = shifter_sticky_stage<IS+1, S, is_signed, Wrapper>(init_sticky, count, fill_bit);
        Wrapper<IS + 1, false> ret;
        if (isRightShift) {
            Wrapper<IS, false> high = shiftstick.template slice<IS, 1>();
            Wrapper<1, false> sticky = shiftstick.template get<0>();
            Wrapper<IS, false> high_reverse = reverse(high);
            ret = high_reverse.concatenate(sticky);
        } else {
            ret = shiftstick;
        }
        return ret;
}
*/

#endif // SHIFTER_STICKY_HPP

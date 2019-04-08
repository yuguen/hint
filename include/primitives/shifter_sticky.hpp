#ifndef SHIFTER_HPP
#define SHIFTER_HPP

#include <iostream>
#include <type_traits>

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "primitives/reverse.hpp"

template<unsigned int S>
struct ShifterStageInfo
{
	static constexpr bool NeedsRecursion = (S>1);
	static constexpr bool IsFinalStage = (S==1);
};

template<unsigned int IS, unsigned int S, template<unsigned int, bool> class Wrapper>
//IS : Input Size (including sticky bit),
//S : size of shift counter
inline Wrapper<IS, false> shifter_sticky_stage(
                Wrapper<IS, false> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
                typename std::enable_if<ShifterStageInfo<S>::NeedsRecursion>::type* = 0,
                typename std::enable_if<((IS-1) >= (1 << (S-1)))>::type * = 0
        )
{
        Wrapper<1<<(S-1), false> padding =  Wrapper<1<<(S-1), false>::generateSequence(fill_bit);
        Wrapper<1, false> stageNeedsShift = count.template get<S-1>();
        Wrapper<S-1, false> countnext = count.template slice<S-2, 0>();

        Wrapper<1, false> sticky_in = input.template get<0>();

        // Incorrect sizes !
        Wrapper<IS - 1 - (1 << (S-1)), false> low = input.template slice<IS - 1 - (1 << (S-1)), 1>();
        Wrapper<(1 << (S-1)), false> high = input.template slice<IS - 1 , IS - (1 << (S-1))>();
        Wrapper<IS, false> next_stage_input;
        if (stageNeedsShift.template isSet<0>()) {
                Wrapper<1, false> cur_sticky = Wrapper<1, false>{sticky_in.template isSet<0>() or high.or_reduce()};
                next_stage_input = Wrapper<1, false>{0}.concatenate(low.concatenate(padding).concatenate(cur_sticky));
        } else {
                next_stage_input = input;
        }
        return shifter_sticky_stage<IS, S-1, Wrapper>(next_stage_input, countnext, fill_bit);
}

template<unsigned int IS, unsigned int S, template<unsigned int , bool> class Wrapper>
inline Wrapper<IS, false> shifter_sticky_stage(
                Wrapper<IS, false> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
                typename std::enable_if<ShifterStageInfo<S>::IsFinalStage>::type* = 0,
                typename std::enable_if<((IS-1) >= (1 << (S-1)))>::type * = 0
        )
{
        Wrapper<IS, false> result;
        if (count.template isSet<0>()) {
                Wrapper<IS - 2, false> low = input.template slice<IS - 2, 1>();
                Wrapper<1, false> sticky_out = input.template isSet<0>() or input.template isSet<IS-1>();
                result = Wrapper<1, false>{0}.concatenate(low.concatenate(fill_bit).concatenate(sticky_out));
        } else {
                result = input;
        }
        return result;
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int, bool> class Wrapper>
inline Wrapper<IS, false> shifter_sticky_stage(
        Wrapper<IS, is_signed> input,
        Wrapper<S, false> count,
        Wrapper<1, false> fill_bit = Wrapper<1, false>{0},
        typename std::enable_if<((IS-1) < (1 << (S-1)))>::type * = 0
    )
{
    constexpr unsigned int nb_null_shift = S - Static_Val<IS-1>::_log2;
    // Incorrect sizes !
    Wrapper<nb_null_shift, false> shift_weights_will_zero = count.template slice<S - 1, S - nb_null_shift>();
    Wrapper<S-nb_null_shift, false> next_count = count.template slice<S-1-nb_null_shift, 0>();

    Wrapper<1, false> stageNeedsShift = shift_weights_will_zero.or_reduce();
    Wrapper<IS, false> ret;
    if (stageNeedsShift) {
        Wrapper<1, false> sticky = input.or_reduce();
        Wrapper<IS-1, false> high = Wrapper<IS-1, false>::generateSequence(fill_bit);
        ret = high.concatenate(sticky);
    } else {
        ret = shifter_sticky_stage<IS, S-nb_null_shift, Wrapper>(input, next_count, fill_bit);
    }
    return ret;
}

template<unsigned int IS, unsigned int S, bool isRightShift, bool is_signed, template<unsigned int , bool> class Wrapper>
inline Wrapper<IS+1, false> shifter_sticky(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
        Wrapper<IS, false> fin_input{isRightShift ? reverse(input) : input};
        Wrapper<IS + 1, false> init_sticky = fin_input.concatenate(Wrapper<1, false>{0});
        Wrapper<IS + 1, false> shiftstick = shifter_sticky_stage<IS+1, S, Wrapper>(init_sticky, count, fill_bit);
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


#endif // SHIFTER_HPP

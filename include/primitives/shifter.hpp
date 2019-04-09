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
Wrapper<IS, false> shifter_stage(
                Wrapper<IS, false> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit,
                typename std::enable_if<ShifterStageInfo<S>::NeedsRecursion>::type* = 0,
                typename std::enable_if<((IS) >= (1 << (S-1)))>::type * = 0
        )
{
        Wrapper<1<<(S-1), false> padding =  Wrapper<1<<(S-1), false>::generateSequence(fill_bit);
        Wrapper<1, false> stageNeedsShift = count.template get<S-1>();
        Wrapper<S-1, false> countnext = count.template slice<S-2, 0>();

        Wrapper<1, false> sticky_in = input.template get<0>();
        Wrapper<IS - (1 << (S-1)), false> low = input.template slice<IS - 1 - (1 << (S-1)), 0>();
        Wrapper<IS, false> next_stage_input;
        if (stageNeedsShift.template isSet<0>()) {
                next_stage_input = low.concatenate(padding);
        } else {
                next_stage_input = input;
        }
        return shifter_stage<IS, S-1, Wrapper>(next_stage_input, countnext, fill_bit);
}

template<unsigned int IS, unsigned int S, template<unsigned int , bool> class Wrapper>
Wrapper<IS, false> shifter_stage(
                Wrapper<IS, false> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit,
                typename std::enable_if<ShifterStageInfo<S>::IsFinalStage>::type* = 0,
                typename std::enable_if<((IS) >= (1 << (S-1)))>::type * = 0
        )
{
        Wrapper<IS, false> result;
        if (count.template isSet<0>()) {
                Wrapper<IS - 1, false> low = input.template slice<IS - 1, 0>();
                result = low.concatenate(fill_bit);
        } else {
                result = input;
        }
        return result;
}

template<unsigned int IS, unsigned int S, bool is_signed, template<unsigned int, bool> class Wrapper>
Wrapper<IS, false> shifter_stage(
        Wrapper<IS, is_signed> input,
        Wrapper<S, false> count,
        Wrapper<1, false> fill_bit,
        typename std::enable_if<((IS) < (1 << (S-1)))>::type * = 0
    )
{
    constexpr unsigned int nb_null_shift = S - Static_Val<IS>::_log2;
    Wrapper<nb_null_shift, false> shift_weights_will_zero = count.template slice<S - 1, S - nb_null_shift>();
    Wrapper<S-nb_null_shift, false> next_count = count.template slice<S-1-nb_null_shift, 0>();
    Wrapper<1, false> stageNeedsShift = shift_weights_will_zero.or_reduce();
    Wrapper<IS, false> ret;
    if (stageNeedsShift) {
        ret = Wrapper<IS, false>::generateSequence(fill_bit);
    } else {
        ret = shifter_stage<IS, S-nb_null_shift, Wrapper>(input, next_count, fill_bit);
    }
    return ret;
}

template< bool isRightShift, unsigned int IS, unsigned int S, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<IS, false> shifter(
                Wrapper<IS, is_signed> input,
                Wrapper<S, false> count,
                Wrapper<1, false> fill_bit = Wrapper<1, false>{0}
    )
{
        Wrapper<IS, false> fin_input{isRightShift ? reverse(input) : input};
        Wrapper<IS + 1, false> shiftstick = shifter_stage<IS, S, Wrapper>(fin_input, count, fill_bit);
        Wrapper<IS + 1, false> ret = isRightShift ? reverse(shiftstick) : shiftstick;
        return ret;
}

#endif // SHIFTER_HPP

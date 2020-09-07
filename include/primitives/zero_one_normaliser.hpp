#ifndef ZERO_ONE_NORMALISER_HPP
#define ZERO_ONE_NORMALISER_HPP

#include <type_traits>
#include "tools/static_math.hpp"
#include "primitives/backwards.hpp"

using std::enable_if;

namespace hint {
template <unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> or_expand_ltr(Wrapper<W, false> const & input,
								Wrapper<1, false> const & prev = {0},
								typename enable_if<(W>1)>::type* = 0)
{
	auto cur = input.template get<W-1>();
	auto res = cur | prev;
	auto next = or_expand_ltr(input.template slice<W-2, 0>(), res);
	return res.concatenate(next);
}

template <unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> or_expand_ltr(Wrapper<W, false> const & input,
								Wrapper<1, false> const & prev = {0},
								typename enable_if<(W==1)>::type* = 0)
{
	return input | prev;
}

template <unsigned int Stage, unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<Stage, false> or_expand_ltr_dupl(Wrapper<W, false> const & input,
								typename enable_if<(Stage==1)>::type* = 0)
{
	return input.or_reduction();
}

template <unsigned int Stage, unsigned int W,  template <unsigned int, bool> class Wrapper>
Wrapper<Stage, false> or_expand_ltr_dupl(Wrapper<W, false> const & input,
								typename enable_if<(Stage <= W and Stage > 1)>::type* = 0)
{
	auto cur = input.template slice<W-1, Stage-1>();
	auto cur_bit = cur.or_reduction();
	return cur_bit.concatenate(or_expand_ltr_dupl<Stage-1>(input));
}

template <unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> or_expand_rtl(Wrapper<W, false> const & input,
								Wrapper<1, false> const & prev = {0},
								typename enable_if<(W>1)>::type* = 0)
{
	auto cur = input.template get<0>();
	auto res = cur | prev;
	auto next = or_expand_ltr(input.template slice<W-2, 0>(), res);
	return next.concatenate(res);
}

template <unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> or_expand_rtl(Wrapper<W, false> const & input,
								Wrapper<1, false> const & prev = {0},
								typename enable_if<(W==1)>::type* = 0)
{
	return input | prev;
}

template<unsigned int W, bool isSigned, template<unsigned int, bool> class Wrapper>
Wrapper<W, false> propagate_leftmost_one(Wrapper<W, isSigned> input)
{
	return or_expand_ltr_dupl<W>(input.as_unsigned());
}

template<unsigned int lvl, unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> shifted_self_ored_rec(
		Wrapper<W, false> in,
		typename enable_if<(lvl >= 1) and (W > 2*lvl)>::type* = 0
		)
{
	auto as_is = in.template slice<W-1, W-lvl>();
	auto to_or_left = in.template slice<W-1, lvl>();
	auto to_or_right = in.template slice<W-1-lvl, 0>();
	auto orred = to_or_left | to_or_right;
	return shifted_self_ored_rec<(lvl<<1)>(as_is.concatenate(orred));
}

template<unsigned int lvl, unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> shifted_self_ored_rec(
		Wrapper<W, false> in,
		typename enable_if<(lvl >= 1) and (W > lvl) and (W <= 2*lvl)>::type* = 0
		)
{
	auto as_is = in.template slice<W-1, W-lvl>();
	auto to_or_left = in.template slice<W-1, lvl>();
	auto to_or_right = in.template slice<W-1-lvl, 0>();
	auto orred = to_or_left | to_or_right;
	return as_is.concatenate(orred);
}

template <unsigned int W, template <unsigned int, bool> class Wrapper>
Wrapper<W, false> shifted_self_ored(
		Wrapper<W, false> in
		)
{
	return shifted_self_ored_rec<1>(in);
}

template<unsigned int W, bool isSigned, template<unsigned int, bool> class Wrapper>
Wrapper<W+1, false> rightmost_indicator(Wrapper<W, isSigned> input)
{
	auto normalised = or_expand_rtl(input.as_unsigned());
	auto reverse_normalised = backwards(normalised);
	auto reversed_indic = reverse_normalised + Wrapper<W, false>{1};
	auto res = backwards(reversed_indic);
	return res;
}

template<unsigned int W, bool isSigned, template<unsigned int, bool> class Wrapper>
Wrapper<W+1, false> leftmost_indicator(Wrapper<W, isSigned> input)
{
	auto normalised = shifted_self_ored(input);
	auto indicator = normalised + Wrapper<W, false>{1};
	return indicator;
}
}


#endif // ZERO_ONE_NORMALISER_HPP

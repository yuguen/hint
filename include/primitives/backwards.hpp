#ifndef BACKWARDS_HPP
#define BACKWARDS_HPP

#include <type_traits>

namespace hint {
	//In case we need both the variable and its reversed value
	//(vivado reverse change the bit order of the value on which its called)
	template <unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
	Wrapper<N, false> backwards(
			Wrapper<N, is_signed> input,
			typename std::enable_if<(N>1)>::type* = 0
		)
	{
		return input.template get<0>().concatenate(backwards(input.template slice<N-1, 1>()));
	}

	template <unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
	Wrapper<N, false> backwards(
			Wrapper<N, is_signed> input,
			typename std::enable_if<(N<=1)>::type* = 0
		)
	{
		return input;
	}
}
#endif // BACKWARDS_HPP

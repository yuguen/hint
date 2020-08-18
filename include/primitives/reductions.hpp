#ifndef REDUCTIONS_HPP
#define REDUCTIONS_HPP

#include <type_traits>

using namespace std;

namespace hint {
	template<unsigned int max_builtin_size, unsigned int N, template<unsigned int, bool> class Wrapper>
	inline Wrapper<1, false> or_reduction_impl(
		Wrapper<N, false> const input,
		typename enable_if<(N>max_builtin_size) and (N%2==1)>::type* = 0
		)
	{
		return or_reduction_impl<max_builtin_size>(
				input.template slice<N-2, (N-1)/2>().bitwise_or(
					input.template slice<((N-1)/2)-1, 0>()
					)
				).bitwise_or(input.template get<N-1>());
	}

	template<unsigned int max_builtin_size, unsigned int N, template<unsigned int, bool> class Wrapper>
	inline Wrapper<1, false> or_reduction_impl(
		Wrapper<N, false> const input,
		typename enable_if<(N>max_builtin_size) and (N%2==0)>::type* = 0
		)
	{
		return or_reduction_impl<max_builtin_size>(
					input.template slice<N-1, N/2>().bitwise_or(
						input.template slice<(N/2)-1, 0>()
						)
					);
	}

	template<unsigned int max_builtin_size, unsigned int N, template<unsigned int, bool> class Wrapper>
	inline Wrapper<1, false> or_reduction_impl(
		Wrapper<N, false> const input,
		typename enable_if<(N<=max_builtin_size) and (N > 1)>::type* = 0
		)
	{
		return input.or_reduction();
	}

	template<unsigned int max_builtin_size, unsigned int N, template<unsigned int, bool> class Wrapper>
	inline Wrapper<1, false> or_reduction_impl(
		Wrapper<N, false> const input,
		typename enable_if<(N==1)>::type* = 0
		)
	{
		return input;
	}



	template<unsigned int max_builtin_size, unsigned int N, bool is_signed, template<unsigned int, bool> class Wrapper>
	inline Wrapper<1, false> or_reduction(
		Wrapper<N, is_signed> const input,
			typename  enable_if<(max_builtin_size >= 1)>::type * = 0
		)
	{
		return or_reduction_impl<max_builtin_size>(input.as_unsigned());
	}
}
#endif //REDUCTIONS_HPP

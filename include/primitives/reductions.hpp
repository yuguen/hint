#ifndef REDUCTIONS_HPP
#define REDUCTIONS_HPP


template<unsigned int N, template<unsigned int, bool> class Wrapper>
inline Wrapper<1, false> or_reduction(
	Wrapper<N, false> const input,
    typename enable_if<(N>1)>::type* = 0
	)
{
	#pragma HLS INLINE
	constexpr unsigned int up_size = (N>>1);
	constexpr unsigned int low_size = N-up_size;
	auto high = input.template slice<N-1, N-up_size>();
	auto low = input.template slice<N-up_size-1, 0>();

	auto high_reduce = or_reduction(high);
	auto low_reduce = or_reduction(low);
	return high_reduce.bitwise_or(low_reduce);
}


template<unsigned int N, template<unsigned int, bool> class Wrapper>
inline Wrapper<1, false> or_reduction(
	Wrapper<N, false> const input,
    typename enable_if<(N==1)>::type* = 0
	)
{
	#pragma HLS INLINE
	return input;
}



template<unsigned int N, bool is_signed, template<unsigned int, bool> class Wrapper>
inline Wrapper<1, false> or_reduction(
	Wrapper<N, is_signed> const input 
	)
{
	#pragma HLS INLINE
	return or_reduction_impl(input.as_unsigned());
}


#endif //REDUCTIONS_HPP
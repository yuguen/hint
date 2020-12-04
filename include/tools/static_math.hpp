#ifndef STATIC_MATH_HPP
#define STATIC_MATH_HPP

#include <cstdint>
namespace hint {
	template <unsigned int num, unsigned int denom>
	struct Static_Ceil_Div{
			static constexpr int val = (num % denom) ? (num / denom) + 1 : num / denom;
	};

	template<unsigned int N, uint8_t remains=0>
	constexpr inline unsigned int ceilLog2()
	{
		return (N <= 1) ? remains : 1 + ceilLog2<(N>>1), remains | (N%2)>();
	}

	template<unsigned int N>
	constexpr inline unsigned int ceil2Power()
	{
		return 1 << ceilLog2<N>();
	}

	template<unsigned int N>
	constexpr inline unsigned int log2()
	{
		return ((N<2) ? 1 : 1+log2<(N>>1)>());
	}

	template<unsigned int N>
	constexpr inline unsigned int r2pow()
	{
		return 1 << log2<N>();
	}

	template <unsigned int N>
	constexpr inline unsigned int floorLog2()
	{
		return (N<= 1) ? 0 : 1 + floorLog2<(N>>1)>();
	}

	template<unsigned int N>
	constexpr bool is2Pow()
	{
		return (1 << ceilLog2<N>()) == N;
	}


	template<unsigned int N>
	constexpr inline bool isOneBelow2Pow()
	{
		return is2Pow<N+1>();
	}

	constexpr unsigned int prod_contrib(unsigned int N)
	{
		return (N <= 1) ? 0 : N; //TODO
	}

	template<unsigned int N>
	class Static_Val
	{
		public:
			static constexpr unsigned int _rlog2 = log2<N>();
			static constexpr unsigned int _r2pow = r2pow<N>();
			static constexpr unsigned int _log2 = ceilLog2<N>();
			static constexpr unsigned int _2pow = ceil2Power<N>();
			static constexpr unsigned int _flog2 = floorLog2<N>();
			static constexpr unsigned int _clog2 = ceilLog2<N>();
			static constexpr unsigned int _storage = ceilLog2<N+1>();
			static constexpr unsigned int _prod_contrib = prod_contrib(N);
			static constexpr bool _is2Pow = is2Pow<N>();
			static constexpr bool _isOneBelow2Pow = isOneBelow2Pow<N>();
	};



	template<unsigned int s1, unsigned int s2>
	class Arithmetic_Prop
	{
		public:
			static constexpr unsigned int _contrib_sum = Static_Val<s1>::_prod_contrib + Static_Val<s2>::_prod_contrib;
			static constexpr unsigned int _prodSize = ( _contrib_sum < 1 ) ? 1 : _contrib_sum;
	};
}
#endif

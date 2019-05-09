#ifndef STATIC_MATH_HPP
#define STATIC_MATH_HPP

#include <cstdint>

template <unsigned int num, unsigned int denom>
struct Static_Ceil_Div{
        static constexpr int val = (num % denom) ? (num / denom) + 1 : num / denom;
};

constexpr unsigned int ceilLog2(unsigned int N, uint8_t remains = 0)
{
        return (N <= 1) ? remains : 1 + ceilLog2(N>>1, remains | (N%2));
}

constexpr unsigned int ceil2Power(unsigned int N)
{
        return 1 << ceilLog2(N);
}

constexpr unsigned int log2(unsigned int N)
{
         return ((N<2) ? 1 : 1+log2(N>>1));
}

constexpr unsigned int r2pow(unsigned int N)
{
         return 1 << log2(N);
}

constexpr unsigned int floorLog2(unsigned int N)
{
    return (N<= 1) ? 0 : 1 + floorLog2(N>>1);
}



template<unsigned int N>
constexpr bool is2Pow()
{
    return (1 << ceilLog2(N)) == N;
}


template<unsigned int N>
constexpr bool isOneBelow2Pow()
{
    return (((1 << ceilLog2(N) ) - 1) == N);
}

template<unsigned int N>
class Static_Val
{
        public:
				static constexpr unsigned int _rlog2 = log2(N);
				static constexpr unsigned int _r2pow = r2pow(N);
				static constexpr unsigned int _log2 = ceilLog2(N);
				static constexpr unsigned int _2pow = ceil2Power(N);
				static constexpr unsigned int _flog2 = floorLog2(N);
				static constexpr unsigned int _clog2 = ceilLog2(N);
				static constexpr unsigned int _storage = ceilLog2(N+1);
                static constexpr bool _is2Pow = is2Pow<N>();
                static constexpr bool _isOneBelow2Pow = isOneBelow2Pow<N>();
};

constexpr unsigned int prod_contrib(unsigned int N)
{
	return (N <= 1) ? 0 : N; //TODO
}

template<unsigned int s1, unsigned int s2>
class Arithmetic_Prop
{
	public:
		static constexpr unsigned int _prodSize = (prod_contrib(s1) + prod_contrib(s2)) < 1 ? 1 : prod_contrib(s1) + prod_contrib(s2);
};
#endif

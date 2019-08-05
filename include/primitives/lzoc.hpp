#ifndef LZOC
#define LZOC

#include "hint.hpp"
#include "tools/static_math.hpp"
// #include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/backwards.hpp"
#include <iostream>

using namespace std;

namespace hint {
	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> getAlmost2PowLZOC(
				Wrapper<N, false> const & input,
				Wrapper<1, false> const & leading,
				typename enable_if<Static_Val<N>::_isOneBelow2Pow and (N>1)>::type* = 0
			)
	{
		constexpr int upper_half = (1 <<Static_Val<N>::_flog2);
		auto upper = input.template slice<N-1, N-upper_half>();
		auto and_red = upper.and_reduction();
		auto or_red = upper.or_reduction();

		auto comp = Wrapper<1, false>::mux(leading, and_red, Wrapper<1, false>{not(or_red).template isSet<0>()});

		auto upper_input = upper.template slice <upper_half - 1, 1>();
		auto low = input.template slice<upper_half-2, 0>();

		auto next_stage_input = Wrapper<upper_half-1, false>::mux(comp, low, upper_input);

		//cerr << to_string(next_stage_input) << endl;

		auto ret = comp.concatenate(getAlmost2PowLZOC(next_stage_input, leading));

		//cerr << to_string(ret) << endl;
		return ret;
	}

	template <template<unsigned int , bool> class Wrapper>
	inline Wrapper<1, false> getAlmost2PowLZOC(
			Wrapper<1, false> const & input,
			Wrapper<1, false> const & leading
		)
	{
		// auto us_val = input;
		return (input == leading);
	}


	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc(
			Wrapper<N, false> const & input,
			Wrapper<1, false> const & leading,
			typename enable_if<(N > 1)>::type* = 0,
			typename enable_if<Static_Val<N>::_isOneBelow2Pow>::type* = 0
		)
	{
		return  getAlmost2PowLZOC(input, leading);
	}

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2 , false> lzoc (
			Wrapper<N, false> const & input,
		Wrapper<1, false> const & leading,
		typename enable_if<(N > 1)>::type* = 0,
		typename enable_if<Static_Val<N>::_is2Pow>::type* = 0
	)
	{

		//cerr << "Call lzoc on 2pow " << endl;
		auto upper = input.template slice<N-1, 1>();
		auto lzocup = getAlmost2PowLZOC(upper, leading);
		auto is_full_one = lzocup.and_reduction();
		auto last_bit_ok = input.template get<0>() == leading;
		auto onezeroseq = Wrapper<1, false>{1}.concatenate(
					Wrapper<Static_Val<N>::_clog2, false>::generateSequence(
						Wrapper<1, false>{0}
						));
		auto uncomplete = Wrapper<1, false>{0}.concatenate(lzocup);

		auto result = Wrapper<Static_Val<N+1>::_clog2, false>::mux(
					is_full_one & last_bit_ok,
					onezeroseq,
					uncomplete
			);
		//cerr << "Value 2pow lzoc : " << to_string(result) << endl;
		return result;
	}

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc (
			Wrapper<N, false> const & input,
		Wrapper<1, false> const & leading,
		typename enable_if<(not Static_Val<N>::_is2Pow)>::type* = 0,
		typename enable_if<(not Static_Val<N>::_isOneBelow2Pow)>::type* = 0,
		typename enable_if<(N > 1)>::type* = 0
	)
	{

		//cerr << "Call Compound lzoc " << N << endl;
		constexpr int upper_size = (1 << Static_Val<N>::_flog2) - 1;
		auto upper = input.template slice<N-1, N-upper_size>();
		auto lzocup = getAlmost2PowLZOC(upper, leading);
		//cerr << "lzocup : " << to_string(lzocup) << endl;

		auto is_full_one = lzocup.and_reduction();
		//cerr << "IFO : " << to_string(is_full_one) << endl;
		auto last_bit_ok = input.template get<N-upper_size-1>() == leading;

		auto msb = is_full_one.bitwise_and(last_bit_ok);

		auto low = input.template slice<N-upper_size - 2, 0>();
		//cerr << "low"<< to_string(low) << endl;
		auto lzoclow = lzoc(low, leading);
		auto ext_lowcount = lzoclow.template leftpad<Static_Val<N>::_flog2>();

		auto lsb = Wrapper<Static_Val<N>::_flog2, false>::mux(
					msb,
					ext_lowcount,
					lzocup
					);

		auto result = msb.concatenate(lsb);
		// cerr << "Value compound lzoc : " << to_string(result) << endl;
		return result;
	}

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	// template<bool is_signed, template<unsigned int , bool> class Wrapper>
	// Wrapper<1, false> lzoc(
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc (
			Wrapper<N, false> const & input,
		Wrapper<1, false> const & leading,
		typename enable_if<(N == 1)>::type* = 0
		)
	{
		//cerr << "Call lzoc on size one" << endl;
		auto res =  getAlmost2PowLZOC(input, leading);
		//cerr << "Res : " << res << endl;
		return res;
	}

	template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc_wrapper (
			Wrapper<N, is_signed> const & input,
		Wrapper<1, false> const & leading
	)
	{

		return lzoc(input.as_unsigned(), leading);

	}
}
#endif // LZOC

#ifndef LZOC
#define LZOC

#include "hint.hpp"
#include "tools/static_math.hpp"
//#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/zero_one_normaliser.hpp"
#include "primitives/indicator_to_index.hpp"
#include "primitives/backwards.hpp"
//#include <iostream>

using namespace std;

namespace hint {
	constexpr unsigned int __hint_lzoc_builtin_limit = 8 * sizeof(int) - 1;

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> getAlmost2PowLZOC(
				Wrapper<N, false> const & input,
				typename enable_if<Static_Val<N>::_isOneBelow2Pow and (N>1)>::type* = 0
			)
	{
		constexpr int upper_half = (1 <<Static_Val<N>::_flog2);
		auto upper = input.template slice<N-1, N-upper_half>();
		auto comp = upper.nor_reduction();
		auto upper_input = upper.template slice <upper_half - 1, 1>();
		auto low = input.template slice<upper_half-2, 0>();

		auto next_stage_input = Wrapper<upper_half-1, false>::mux(comp, low, upper_input);

		//cerr << to_string(next_stage_input) << endl;

		auto ret = comp.concatenate(getAlmost2PowLZOC(next_stage_input));

		//cerr << to_string(ret) << endl;
		return ret;
	}

	template <template<unsigned int , bool> class Wrapper>
	inline Wrapper<1, false> getAlmost2PowLZOC(
			Wrapper<1, false> const & input
		)
	{
		// auto us_val = input;
		return (input.invert());
	}


	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc(
			Wrapper<N, false> const & input,
			typename enable_if<(N > __hint_lzoc_builtin_limit)>::type* = 0,
			typename enable_if<Static_Val<N>::_isOneBelow2Pow>::type* = 0
		)
	{
		return  getAlmost2PowLZOC(input);
	}

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc (
			Wrapper<N, false> const & input,
		typename enable_if<(not Static_Val<N>::_isOneBelow2Pow)>::type* = 0,
		typename enable_if<(N > __hint_lzoc_builtin_limit)>::type* = 0
	)
	{
		constexpr unsigned int lzoc_size = Static_Val<N>::_storage;
		constexpr unsigned int count_size = (1 << lzoc_size) - 1;
		constexpr unsigned int pad_width = count_size - N;
		auto padding = Wrapper<pad_width, false>::generateSequence({{1}});
		auto padded = input.concatenate(padding);
		// cerr << "Value compound lzoc : " << to_string(result) << endl;
		return getAlmost2PowLZOC(padded);
	}

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc (
			Wrapper<N, false> const & input,
		typename enable_if<(not Static_Val<N>::_isOneBelow2Pow)>::type* = 0,
		typename enable_if<(N > 1 and N <= __hint_lzoc_builtin_limit)>::type* = 0
	)
	{
		constexpr unsigned int pad_width = __hint_lzoc_builtin_limit + 1 - N;
		auto padding = Wrapper<pad_width, false>::generateSequence({{1}});
        auto padded = input.concatenate(padding);
        // cerr << "Value compound lzoc : " << to_string(result) <<
        // endl;
        return {static_cast<typename Wrapper<Static_Val<N + 1>::_clog2,
                                                false>::storage_type>(
            __builtin_clz(padded.unravel()))};
    }

	template<unsigned int N, template<unsigned int , bool> class Wrapper>
	// template<bool is_signed, template<unsigned int , bool> class Wrapper>
	// Wrapper<1, false> lzoc(
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc (
			Wrapper<N, false> const & input,
		typename enable_if<(N == 1)>::type* = 0
		)
	{
		//cerr << "Call lzoc on size one" << endl;
		auto res =  input.invert();
		//cerr << "Res : " << res << endl;
		return res;
	}

	template<unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
	inline Wrapper<Static_Val<N+1>::_clog2, false> lzoc_wrapper (
			Wrapper<N, is_signed> const & input,
			Wrapper<1, false> const & leading
	)
	{
		auto real_input = Wrapper<N, false>::mux(leading, input.invert(), input.as_unsigned());
		//cerr << "Real input : " << endl << to_string(real_input) << endl;
		//auto prefix = propagate_leftmost_one(real_input);
		//cerr << "Prefix : " << endl << to_string(prefix) << endl;
		//auto indicator = prefix + Wrapper<N, false>{1};
		//cerr << "Indicator :" << endl << to_string(indicator) << endl;
		//auto res = indicator_to_idx(indicator);
		//cerr << "res :" << endl << to_string(res) << endl;
		//return res;
		return lzoc(real_input);
		//return fast_lzc(real_input);
	}
}
#endif // LZOC

#ifndef INDICATOR_TO_INDEX_HPP
#define INDICATOR_TO_INDEX_HPP

#include <type_traits>
//#include <iostream>

#include "tools/static_math.hpp"
#include "primitives/multi_result_holder.hpp"
#include "tools/int_sequence.hpp"
#include "primitives/useful_functor.hpp"
#include "tools/printing.hpp"

using std::enable_if;
//using std::cerr;
//using std::endl;

namespace hint {

template<unsigned int valSize, template<unsigned int, bool> class Wrapper>
using one_bit_id_res_t = MultiResultHolder<Static_Val<valSize-1>::_storage, valSize, Wrapper>;


template<unsigned int curVal, unsigned int W, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W-1>::_storage, false> one_bit_identifier(
		Wrapper<W, false> input,
		typename enable_if<(curVal < W)>::type* = 0)
{
	auto indicator = input.template get<curVal>();
	auto mask = Wrapper<Static_Val<W-1>::_storage, false>::generateSequence(indicator);
	auto local_idx = Wrapper<Static_Val<W-1>::_storage, false>{curVal};
	auto res = local_idx & mask;
	return res;
}

template<unsigned int W, unsigned int bitPos>
struct BitSelectWeightCond {
	static constexpr unsigned int bit_weight = 1 << bitPos;
	static constexpr unsigned int modulus = 1 << (bitPos + 1);
	static constexpr bool check(const unsigned idx) {
		return (((W - (idx+1)) % modulus) >= bit_weight);
	}
};

/**
 * input is an indicator string : rightmost bit set for 1, leftmost bit for N
 */
template<unsigned int bs, unsigned int N, template<unsigned int, bool> class Wrapper>
inline Wrapper<bs+1, false> get_indicator_bit(Wrapper<N, false> const & input,
	   typename enable_if<((Static_Val<N>::_storage >= bs) and (bs > 0))>::type* = 0
	)
{
	//cerr << "Selecting bit " << bs << endl;
	using uf_seq = make_sequence<N>;
	using filter = BitSelectWeightCond<N, bs>;
	using filtered = call<FilterSeq<filter, uf_seq>>;
	auto selorred = input.select_or_reduce(filtered{});
	//cerr << "selorred :" << endl << to_string(selorred) << endl;
	return selorred.concatenate(get_indicator_bit<bs-1>(input));
}

template<unsigned int bs, unsigned int N, template<unsigned int, bool> class Wrapper>
inline Wrapper<bs+1, false> get_indicator_bit(Wrapper<N, false> const & input,
	   typename enable_if<(bs == 0)>::type* = 0
	)
{
	using uf_seq = make_sequence<N>;
	using filter = BitSelectWeightCond<N, bs>;
	using filtered = call<FilterSeq<filter, uf_seq>>;
	return input.select_or_reduce(filtered{});
}

template<unsigned int W, template<unsigned int, bool> class Wrapper>
inline Wrapper<Static_Val<W>::_storage, false> indicator_to_idx(Wrapper<W, false> const & indicator)
{
	//auto indic_table = build_idx_table(indicator);
	//auto idx = reduce<OrReduce>(indic_table);
	constexpr unsigned int csize = Static_Val<W-1>::_storage;
	//cerr << "W : " << W << endl << "csize : " << csize << endl << "Indicator : " << to_string(indicator) << endl;
	return get_indicator_bit<csize-1>(indicator);
}

template<unsigned int W, template<unsigned int, bool> class Wrapper>
inline Wrapper<Static_Val<W-1>::_storage, false> indicator_to_complement(Wrapper<W, false> indicator)
{
	auto indic_table = build_complement_table(indicator);
	auto idx = reduce<OrReduce>(indic_table);
	return idx;
}



}
#endif // INDICATOR_TO_INDEX_HPP

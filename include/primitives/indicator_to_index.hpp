#ifndef INDICATOR_TO_INDEX_HPP
#define INDICATOR_TO_INDEX_HPP

#include <type_traits>

#include "tools/static_math.hpp"
#include "primitives/multi_result_holder.hpp"
#include "tools/int_sequence.hpp"
#include "primitives/useful_functor.hpp"

using std::enable_if;

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

template <unsigned int W, template<unsigned int, bool> class Wrapper, unsigned int... Indices>
one_bit_id_res_t<W, Wrapper> build_idx_table_exp(
        Wrapper<W, false> indicator,
        UISequence<Indices...>
        )
{
    return {one_bit_identifier<Indices>(indicator)...};
}

template <unsigned int W, template<unsigned int, bool> class Wrapper>
one_bit_id_res_t<W, Wrapper> build_idx_table(
        Wrapper<W, false> indicator
    )
{
    using seq = make_sequence<W>;
    return build_idx_table_exp(indicator, seq{});
}

template<unsigned int curVal, unsigned int W, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W-1>::_storage, false> one_bit_complement(
        Wrapper<W, false> input,
        typename enable_if<(curVal < W)>::type* = 0)
{
    auto indicator = input.template get<curVal>();
    auto mask = Wrapper<Static_Val<W-1>::_storage, false>::generateSequence(indicator);
    auto local_idx = Wrapper<Static_Val<W-1>::_storage, false>{W - 1 - curVal};
    auto res = local_idx & mask;
    return res;
}

template <unsigned int W, template<unsigned int, bool> class Wrapper, unsigned int... Indices>
one_bit_id_res_t<W, Wrapper> build_complement_table_exp(
        Wrapper<W, false> indicator,
        UISequence<Indices...>
        )
{
    return {one_bit_complement<Indices>(indicator)...};
}

template <unsigned int W, template<unsigned int, bool> class Wrapper>
one_bit_id_res_t<W, Wrapper> build_complement_table(
        Wrapper<W, false> indicator
    )
{
    using seq = make_sequence<W>;
    return build_complement_table_exp(indicator, seq{});
}


template<unsigned int W, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W-1>::_storage, false> indicator_to_idx(Wrapper<W, false> indicator)
{
    auto indic_table = build_idx_table(indicator);
    auto idx = reduce<OrReduce>(indic_table);
    return idx;
}

template<unsigned int W, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W-1>::_storage, false> indicator_to_complement(Wrapper<W, false> indicator)
{
    auto indic_table = build_complement_table(indicator);
    auto idx = reduce<OrReduce>(indic_table);
    return idx;
}

}
#endif // INDICATOR_TO_INDEX_HPP

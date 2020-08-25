#ifndef MULTI_RESULT_HOLDER_HPP
#define MULTI_RESULT_HOLDER_HPP

#include <type_traits>
#include "tools/int_sequence.hpp"

using std::enable_if;

namespace hint {
template<unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
struct MultiResultHolder
{
    static constexpr unsigned int _value_width = Width;
    using value_type = Wrapper<_value_width, false>;
    static constexpr unsigned int _nb_values = Len;
    value_type values[_nb_values];
};

template<unsigned int ColIdx, unsigned int lineStart, unsigned int lineEnd,  unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
inline Wrapper<lineEnd - lineStart + 1, false> col_to_line_rec(
        MultiResultHolder<Width, Len, Wrapper> & input,
        typename enable_if<(lineEnd > lineStart)>::type* = 0
    )
{
    constexpr unsigned int size = lineEnd - lineStart + 1;
    constexpr unsigned int half_size = size >> 1;
    constexpr unsigned int mid_idx = lineStart + half_size - 1;
    return col_to_line_rec<ColIdx, lineStart, mid_idx>(input).concatenate(col_to_line_rec<ColIdx, mid_idx+1, lineEnd>(input));
}

template<unsigned int ColIdx, unsigned int lineStart, unsigned int lineEnd,  unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
inline Wrapper<lineEnd - lineStart + 1, false> col_to_line_rec(
        MultiResultHolder<Width, Len, Wrapper> & input,
        typename enable_if<(lineEnd == lineStart)>::type* = 0
    )
{
    return input.values[lineStart].template get<ColIdx>();
}

template<unsigned int ColIdx,  unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
inline Wrapper<Len, false> col_to_line(MultiResultHolder<Width, Len, Wrapper> & input)
{
    return col_to_line_rec<ColIdx, 0, Len-1>(input);
}

template<unsigned int Width, unsigned int Len, template <unsigned int, bool> class Wrapper, unsigned int... Indices>
inline MultiResultHolder<Len, Width, Wrapper> transpose_exp(MultiResultHolder<Width, Len, Wrapper> & input,
                                                     UISequence<Indices...>)
{
    return {col_to_line<Indices>(input)...};
}

template<unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
inline MultiResultHolder<Len, Width, Wrapper> transpose(MultiResultHolder<Width, Len, Wrapper> & input)
{
    using seq = make_reverse_sequence<Width>;
    return transpose_exp(input, seq{});
}

template<template<unsigned int> class Functor, unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper, unsigned int... Indices>
inline MultiResultHolder<Functor<Width>::res_width, Len, Wrapper> apply_elem_func_exp(
        MultiResultHolder<Width, Len, Wrapper> & input,
        UISequence<Indices...>)
{
    return MultiResultHolder<Functor<Width>::res_width, Len, Wrapper>{{Functor<Width>::apply(input.values[Indices])...}};
}

template<template<unsigned int> class Functor, unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
MultiResultHolder<Functor<Width>::res_width, Len, Wrapper> apply_elem_func(MultiResultHolder<Width, Len, Wrapper> & input)
{
    using seq = make_sequence<Len>;
    return apply_elem_func_exp<Functor>(input, seq{});
}



template <template <unsigned int, unsigned int> class Reducer, unsigned int lineStart, unsigned int lineEnd, unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
Wrapper<Reducer<Width, lineEnd-lineStart + 1>::res_width, false> reduce_req(
        MultiResultHolder<Width, Len, Wrapper>& input,
        typename enable_if<(lineEnd > lineStart) and (lineEnd < Len)>::type* = 0)
{
    constexpr unsigned int size = lineEnd - lineStart + 1;
    constexpr unsigned int first_half = size >> 1;
    constexpr unsigned int mid = lineStart + first_half - 1;
    auto up = reduce_req<Reducer, lineStart, mid>(input);
    auto down = reduce_req<Reducer, mid + 1, lineEnd>(input);
    return Reducer<Width, size>::reduce(up, down);
}

template <template <unsigned int, unsigned int> class Reducer, unsigned int lineStart, unsigned int lineEnd, unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
Wrapper<Reducer<Width, lineEnd-lineStart + 1>::res_width, false> reduce_req(
        MultiResultHolder<Width, Len, Wrapper>& input,
        typename enable_if<(lineEnd == lineStart) and (lineEnd < Len)>::type* = 0)
{
    return input.values[lineStart];
}


template <template <unsigned int, unsigned int> class Reducer, unsigned int Width, unsigned int Len, template<unsigned int, bool> class Wrapper>
Wrapper<Reducer<Width, Len>::res_width, false> reduce(MultiResultHolder<Width, Len, Wrapper>& input)
{
    return reduce_req<Reducer, 0, Len-1>(input);
}
}


#endif // MULTI_RESULT_HOLDER_HPP

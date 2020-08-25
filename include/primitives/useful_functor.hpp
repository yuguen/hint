#ifndef USEFUL_FUNCTOR_HPP
#define USEFUL_FUNCTOR_HPP

#include "primitives/multi_result_holder.hpp"

namespace hint {
template<unsigned int Width>
struct ElemWiseOrReduce {
    static constexpr unsigned int res_width = 1;
    template<template<unsigned int, bool> class Wrapper>
    static Wrapper<res_width, false> apply(Wrapper<Width, false> input) {
        return input.or_reduction();
    }
};

template <unsigned int Width, unsigned int Length>
struct ConcatReduce {
    static constexpr unsigned int res_width = Width * Length;
    static constexpr unsigned int input_width = res_width >> 1;

    template<unsigned int i1, unsigned int i2, template<unsigned int, bool> class Wrapper>
    static Wrapper<res_width, false> reduce(Wrapper<i1, false> in1, Wrapper<i2, false> in2)
    {
        return in1.concatenate(in2);
    }
};

template <unsigned int Width, unsigned int Length>
struct OrReduce {
    static constexpr unsigned int res_width = Width;

    template<template<unsigned int, bool> class Wrapper>
    static Wrapper<res_width, false> reduce(Wrapper<Width, false> in1, Wrapper<Width, false> in2)
    {
        return in1 | in2;
    }
};
}

#endif // USEFUL_FUNCTOR_HPP

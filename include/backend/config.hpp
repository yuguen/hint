#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace hint {
template<template<unsigned int, bool> class Wrapper>
struct Config_Values
{
    constexpr static unsigned int shift_group_by = 1;
};
}


#endif // CONFIG_HPP

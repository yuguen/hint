#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <iostream>
#include <string>
#include <type_traits>

using namespace std;
#include "hint.hpp"

template <typename val, unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
struct printer{
    inline constexpr void operator()(stringstream & s, hint_base<W, is_signed, wrapper> const & signal)
    {
        s << (signal.template isSet<val::value>() ? '1' : '0');
        printer<integral_constant<unsigned int, val::value-1>, W, is_signed, wrapper>{}(s, signal);
    }
};

template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
struct printer<integral_constant<unsigned int, 0>, W, is_signed, wrapper>
{
    inline constexpr void operator()(stringstream& s, hint_base<W, is_signed, wrapper> const & signal)
    {
        s << (signal.template isSet<0>() ? '1' : '0');
    }
};


template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
string to_string(hint_base<W, is_signed, wrapper> const & signal)
{
    stringstream s{};
    printer<integral_constant<unsigned int, W-1>, W, is_signed, wrapper>{}(s, signal);
    return s.str();
}

#endif // PRINTING_HPP

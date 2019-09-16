#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <sstream>
#include <string>
#include <type_traits>

using namespace std;
#include "hint.hpp"
namespace hint {
    template <unsigned int C, unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    void printer(stringstream& s, wrapper<W, is_signed> const & signal,
            typename enable_if<(C>0)>::type* = 0){
            s << (signal.template isSet<C>() ? '1' : '0') << printer<C-1, W, is_signed, wrapper>(s, signal);
    };

    template <unsigned int C, unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    void printer(stringstream& s, wrapper<W, is_signed> const & signal,
            typename enable_if<(C==0)>::type* = 0){
            s << (signal.template isSet<C>() ? '1' : '0');
    };


    template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    string to_string(wrapper<W, is_signed> const & signal)
    {
        stringstream s{};
        printer<W-1, W, is_signed, wrapper>(s, signal);
        return s.str();
    }
}

#endif // PRINTING_HPP

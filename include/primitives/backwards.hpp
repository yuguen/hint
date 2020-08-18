#ifndef BACKWARDS_HPP
#define BACKWARDS_HPP

#include <type_traits>

using std::enable_if;

namespace hint {

    template<unsigned int idx, unsigned int N, template<unsigned int , bool> class Wrapper>
    inline void do_backward(Wrapper<N, false> const & input,
                       Wrapper<N, false> & output,
                       typename enable_if<(idx > 0)>::type* = 0)
    {
        output.template affect_bit<idx, N-1-idx>(input);
        do_backward<idx-1>(input, output);
    }

    template<unsigned int idx, unsigned int N, template<unsigned int , bool> class Wrapper>
    inline void do_backward(Wrapper<N, false> const & input,
                       Wrapper<N, false> & output,
                       typename enable_if<(idx == 0)>::type* = 0)
    {
        output.template affect_bit<idx, N-1-idx>(input);
    }


	template <unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
    inline Wrapper<N, false> backwards(
            Wrapper<N, is_signed> const input
		)
    {
       auto usi = input.as_unsigned();
       Wrapper<N, false> result;
       do_backward<N-1>(usi, result);
       return result;
    }
}
#endif // BACKWARDS_HPP

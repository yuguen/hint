#ifndef REVERSE_HPP
#define REVERSE_HPP

//In case we need both the variable and its reversed value
//(vivado reverse change the bit order of the value on which its called)
template <unsigned int N, bool is_signed, template<unsigned int , bool> class Wrapper>
Wrapper<N, false> reverse(
        Wrapper<N, is_signed> input
    )
{
    return input.reverse();
}


#endif // REVERSE_HPP

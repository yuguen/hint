#ifndef REVERSE_HPP
#define REVERSE_HPP

//In case we need both the variable and its reversed value
//(vivado reverse change the bit order of the value on which its called)
template <unsigned int N, template<unsigned int , bool> class Wrapper>
Wrapper<N, false> reverse(
        Wrapper<N, false> input,
        typename std::enable_if<(N>1)>::type* = 0
    )
{
    return (input.template get<0>()).concatenate(
                reverse(
                    static_cast<Wrapper<N-1, false> >(
                        input.template slice<N-1, 1>()
                        )
                    )
                );
}

template <unsigned int N, template<unsigned int , bool> class Wrapper>
Wrapper<N, false> reverse(
        Wrapper<N, false> input,
        typename std::enable_if<(N==1)>::type* = 0
    )
{
    return input;
}


#endif // REVERSE_HPP

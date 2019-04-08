#ifndef REVERSE_HPP
#define REVERSE_HPP

//In case we need both the variable and its reversed value
//(vivado reverse change the bit order of the value on which its called)
template <int N, template<unsigned int , bool> class Wrapper>
inline Wrapper<N, false> reverse(
        Wrapper<N, false> input,
        typename std::enable_if<(N>1)>::type* = 0
    )
{
    return static_cast<Wrapper<1, false> >(input.get<0>()).concatenate(
                reverse(
                    static_cast<Wrapper<N-1, false> >(
                        input.slice<N-1, 1>()
                        )
                    )
                );
}

template <int N>
inline Wrapper<N, false> reverse(
        Wrapper<N, false> input,
        typename std::enable_if<(N==1)>::type* = 0
    )
{
    return input;
}


#endif // REVERSE_HPP
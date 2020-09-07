#ifndef INT_SEQUENCE_HPP
#define INT_SEQUENCE_HPP

#include <type_traits>

using std::enable_if;

template <unsigned int VAL>
struct STToType
{
		static constexpr unsigned int val = VAL;
		using type = STToType;
};

template<class T> using call = typename T::type;

template <typename... elems>
struct Sequence {
		using type = Sequence;
};

template <unsigned int shiftSize, typename Val>
struct Shifter;

template <unsigned int shiftSize, unsigned int Val>
struct Shifter<shiftSize, STToType<Val>>
{
		using type = STToType<Val + shiftSize>;
};

template<unsigned int shiftSize, typename... elems>
struct Shifter<shiftSize, Sequence<elems...> >
{
		using type = Sequence<call<Shifter<shiftSize, elems>>...>;
};

template<unsigned int shiftSize>
struct Shifter<shiftSize, Sequence<>>
{
		using type = Sequence<>;
};

template<typename T1, typename T2> struct Merger;
template<typename... elS1, typename... elS2>
struct Merger<Sequence<elS1...>, Sequence<elS2...>> {
		using type = Sequence<elS1..., elS2...>;
};

template <typename>
struct Unfold;

template<unsigned int shift, typename T1, typename T2>
struct shiftConcat;

template<unsigned int shift, typename... elS1, typename... elS2>
struct shiftConcat<shift, Sequence<elS1...>, Sequence<elS2...>>
{
		using shiftS2 = call<Shifter<sizeof...(elS1), Sequence<elS2...>>>;
		using type = call<Merger<Sequence<elS1...>, shiftS2>>;
};


template <typename>
struct GenSeqElem;

template <>
struct GenSeqElem<STToType<0>> : Sequence<>{};
template <>
struct GenSeqElem<STToType<1>> : Sequence<STToType<0>>{};

template <unsigned int val> using GenSeqCall = call<GenSeqElem<STToType<val>>>;

template <unsigned int NElem>
struct GenSeqElem<STToType<NElem>> : shiftConcat<NElem/2, GenSeqCall<NElem/2>, GenSeqCall<NElem-NElem/2>>{};

template <typename>
struct UnWrapper;

template <unsigned int...>
struct UISequence{};

template <unsigned int... values>
struct UnWrapper<Sequence<STToType<values>...>> {
		using type = UISequence<values...>;
};

template <typename T>
struct ReverseSeq;

template <unsigned int... values>
struct ReverseSeq<UISequence<values...>>
{
	using type = UISequence<(sizeof... (values)-1-values)...>;
};

template<unsigned int... el1, unsigned int... el2>
struct Merger<UISequence<el1...>, UISequence<el2...>> {
		using type = UISequence<el1..., el2...>;
};

template<unsigned int... el1>
struct Merger<UISequence<el1...>, UISequence<>> {
		using type = UISequence<el1...>;
};

template<typename... el1>
struct Merger<Sequence<el1...>, Sequence<>> {
		using type = Sequence<el1...>;
};

template<typename el1, typename... el2>
struct Merger<el1, Sequence<el2...>> {
		using type = Sequence<el1, el2...>;
};

template<unsigned int val, unsigned int... els>
struct Merger<STToType<val>, UISequence<els...>> {
		using type = UISequence<val, els...>;
};

template <typename filter, typename T, typename Enable = void>
struct FilterSeq;

/**
 * FilterSeq on UISequence
 */
template<typename Condition, unsigned int head, unsigned int... elems>
struct FilterSeq<Condition, UISequence<head, elems...>, typename enable_if<Condition::check(head)>::type> {
	using type = call<Merger<STToType<head>, call<FilterSeq<Condition, UISequence<elems...>>>>>;
};

template<typename Condition, unsigned int head, unsigned int... elems>
struct FilterSeq<Condition, UISequence<head, elems...>, typename enable_if<not Condition::check(head)>::type> {
	using type = call<FilterSeq<Condition, UISequence<elems...>>>;
};

template<typename Condition, unsigned int head>
struct FilterSeq<Condition, UISequence<head>, typename enable_if<Condition::check(head)>::type> {
	using type = UISequence<head>;
};

template<typename Condition, unsigned int head>
struct FilterSeq<Condition, UISequence<head>, typename enable_if<not Condition::check(head)>::type> {
	using type = UISequence<>;
};

/***
 * FilterSeq on Sequence
 */

template<typename Condition, typename head>
struct FilterSeq<Condition, Sequence<head>, typename enable_if<Condition::check(head::val)>::type> {
	using type = Sequence<head>;
};

template<typename Condition, typename head>
struct FilterSeq<Condition, Sequence<head>, typename enable_if<not Condition::check(head::val)>::type> {
	using type = Sequence<>;
};

template<typename Condition, typename head, typename... elems>
struct FilterSeq<Condition, Sequence<head, elems...>, typename enable_if<Condition::check(head::val)>::type> {
	using next_type = Sequence<elems...>;
	using type = call<Merger<head, call<FilterSeq<Condition, next_type>>>>;
};

template<typename Condition, typename head, typename... elems>
struct FilterSeq<Condition, Sequence<head, elems...>, typename enable_if<not Condition::check(head::val)>::type> {
	using next_type = Sequence<elems...>;
	using type = call<FilterSeq<Condition, next_type>>;
};

template <unsigned int NElem>
using make_sequence = call<UnWrapper<GenSeqCall<NElem>>>;

template <unsigned int NElem>
using make_reverse_sequence = call<ReverseSeq<make_sequence<NElem>>>;


#endif // INT_SEQUENCE_HPP

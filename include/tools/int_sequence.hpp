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
struct FilterHeadSeq;

/****************************************************************
 * FilterHeadSeq on Sequence
 */

template <typename filter, typename head, typename... elems>
struct FilterHeadSeq<filter, Sequence<head, elems...>, typename enable_if<filter::check(head::val)>::type>
{
	using top = Sequence<head>;
	using tail = Sequence<elems...>;
};

template <typename filter, typename head, typename... elems>
struct FilterHeadSeq<filter, Sequence<head, elems...>, typename enable_if<!filter::check(head::val)>::type>
{
	using top = Sequence<>;
	using tail = Sequence<elems...>;
};

template <typename filter, typename head>
struct FilterHeadSeq<filter, Sequence<head>, typename enable_if<filter::check(head::val)>::type>
{
	using top = Sequence<head>;
	using tail = Sequence<>;
};

template <typename filter, typename head>
struct FilterHeadSeq<filter, Sequence<head>, typename enable_if<!filter::check(head::val)>::type>
{
	using top = Sequence<>;
	using tail = Sequence<>;
};

/**************************************************************************
 *  FilterHeadSeq on UISequence
 */

template <typename filter, unsigned int head, unsigned int... elems>
struct FilterHeadSeq<filter, UISequence<head, elems...>, typename enable_if<filter::check(head)>::type>
{
	using top = UISequence<head>;
	using tail = UISequence<elems...>;
};

template <typename filter, unsigned int head, unsigned int... elems>
struct FilterHeadSeq<filter, UISequence<head, elems...>, typename enable_if<!filter::check(head)>::type>
{
	using top = UISequence<>;
	using tail = UISequence<elems...>;
};

template <typename filter, unsigned int head>
struct FilterHeadSeq<filter, UISequence<head>, typename enable_if<filter::check(head)>::type>
{
	using top = UISequence<head>;
	using tail = UISequence<>;
};

template <typename filter, unsigned int head>
struct FilterHeadSeq<filter, UISequence<head>, typename enable_if<!filter::check(head)>::type>
{
	using top = UISequence<>;
	using tail = UISequence<>;
};


template <typename filter, typename T>
struct FilterSeq;

/*************************************************************
 * FilterSeq on UISequence
 */
template<typename Condition>
struct FilterSeq<Condition, UISequence<>>
{
	using type = UISequence<>;
};

template<typename Condition, unsigned int... elems>
struct FilterSeq<Condition, UISequence<elems...>> {
	using seq_type = UISequence<elems...>;
	using filtered_head_type = FilterHeadSeq<Condition, seq_type>;
	using top = typename filtered_head_type::top;
	using unfiltered_tail = typename filtered_head_type::tail;
	using filtered_tail = call<FilterSeq<Condition, unfiltered_tail>>;
	using type = call<Merger<top, filtered_tail>>;
};

/*************************************************************
 * FilterSeq on Sequence
 */
template<typename Condition>
struct FilterSeq<Condition, Sequence<>>
{
	using type = Sequence<>;
};

template<typename Condition, typename... elems>
struct FilterSeq<Condition, Sequence<elems...>> {
	using seq_type = Sequence<elems...>;
	using filtered_head_type = FilterHeadSeq<Condition, seq_type>;
	using top = typename filtered_head_type::top;
	using unfiltered_tail = typename filtered_head_type::tail;
	using filtered_tail = call<FilterSeq<Condition, unfiltered_tail>>;
	using type = call<Merger<top, filtered_tail>>;
};

/**********************************************************/

template <unsigned int NElem>
using make_sequence = call<UnWrapper<GenSeqCall<NElem>>>;

template <unsigned int NElem>
using make_reverse_sequence = call<ReverseSeq<make_sequence<NElem>>>;


#endif // INT_SEQUENCE_HPP

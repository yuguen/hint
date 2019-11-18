#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <type_traits>

#include "static_math.hpp"

namespace hint {
	template<unsigned int... Seq>
	struct Sequence{};

	template<typename Seq1Type, typename Seq2Type>
	struct SequenceMerger;

	template <unsigned int... Seq1Members, unsigned int... Seq2Members>
	struct SequenceMerger<Sequence<Seq1Members...>, Sequence<Seq2Members...>>
	{
		using type = Sequence<Seq1Members..., Seq2Members...>;
	};

	template<unsigned int, typename>
	struct SequenceShifter;

	template<unsigned int Shift, unsigned int... SeqMembers>
	struct SequenceShifter<Shift, Sequence<SeqMembers...>>
	{
		using type = Sequence<(SeqMembers + Shift)...>;
	};

	template<unsigned int SeqLength>
	struct _Seq2Pow
	{
		private:
			static constexpr unsigned int HalfLength = SeqLength >> 1;
			using subseq = typename _Seq2Pow<HalfLength>::type;
		public:
			using type = typename SequenceMerger<subseq, typename SequenceShifter<HalfLength, subseq>::type>::type;
	};

	template<>
	struct _Seq2Pow<1>
	{
		using type = Sequence<0>;
	};

	template<unsigned int SeqLength, class Enable = void>
	struct SequenceBuilder;

	template<unsigned int SeqLength>
	struct SequenceBuilder<SeqLength, typename std::enable_if<is2Pow<SeqLength>()>::type>
	{
		using type = typename _Seq2Pow<SeqLength>::type;
	};

	template<unsigned int SeqLength>
	struct SequenceBuilder<SeqLength, typename std::enable_if<not is2Pow<SeqLength>()>::type>
	{
		private:
			static constexpr unsigned int Low2Pow = 1 << Static_Val<SeqLength>::_flog2;
			using _2pow_seq = typename _Seq2Pow<Low2Pow>::type;
			using _remain_seq = typename SequenceShifter<Low2Pow, typename SequenceBuilder<SeqLength - Low2Pow>::type>::type;
		public:
			using type = typename SequenceMerger<_2pow_seq, _remain_seq>::type;
	};


}

#endif // SEQUENCE_HPP

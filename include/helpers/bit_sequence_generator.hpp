#ifndef HINT_BIT_SEQUENCE_GENERATOR
#define HINT_BIT_SEQUENCE_GENERATOR

#include <type_traits>

#include "primitives/table.hpp"

using std::enable_if;

namespace hint {
	template<unsigned int SeqLength, template<unsigned int, bool> class Wrapper>
	struct _OneZeroSeqMapper {
		template<unsigned int key>
		static inline Wrapper<SeqLength, false> map(
				typename enable_if<(key > 0)>::type* = 0
				)
		{
			static_assert (key < SeqLength, "Key should be smaller than sequence length");
			constexpr unsigned int one_length = SeqLength - key;
			return Wrapper<one_length, false>::generateSequence({1}).concatenate(
						Wrapper<key, false>::generateSequence({0})
				);
		}

		template<unsigned int key>
		static inline Wrapper<SeqLength, false> map(
				typename enable_if<(key == 0)>::type* = 0
				)
		{
			return Wrapper<SeqLength, false>::generateSequence({1});
		}
	};

	template<unsigned int KeySize, unsigned int ValueSize, template<unsigned int, bool> class Wrapper>
	inline Wrapper<ValueSize, false> one_then_zeros(Wrapper<KeySize, false> number_of_zeros)
	{
		static_assert (ValueSize >= (1 << KeySize), "ValueSize should be bigger than key size");
		return TabulatedFunction<_OneZeroSeqMapper<ValueSize, Wrapper>, KeySize, ValueSize, Wrapper>::read(number_of_zeros);
	}

}

#endif
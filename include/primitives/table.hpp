#ifndef TABLE_HPP
#define TABLE_HPP

#include "hint.hpp"

namespace hint {
	template<
			class mapping,
			unsigned int KeySize,
			unsigned int ValueSize,
			unsigned int level,
			unsigned int lowerBitsVal,
			template<unsigned int, bool> class Wrapper>
	inline Wrapper<ValueSize, false> _table_func_impl(
			Wrapper<KeySize, false> const & key,
			typename enable_if<(level < KeySize-1)>::type* = 0)
	{
		constexpr unsigned int one_val = (1 << level) | lowerBitsVal;
		return Wrapper<ValueSize, false>::mux(
					key.template get<level>(),
					_table_func_impl<mapping, KeySize, ValueSize, level + 1, one_val>(key),
					_table_func_impl<mapping, KeySize, ValueSize, level + 1, lowerBitsVal>(key)
			);
	}

	template <
				class mapping,
				unsigned int KeySize,
				unsigned int ValueSize,
				unsigned int level,
				unsigned int lowerBitsVal,
				template<unsigned int, bool> class Wrapper>
	inline Wrapper<ValueSize, false> _table_func_impl(
			Wrapper<KeySize, false> const & key ,
			typename enable_if<(level == (KeySize-1))>::type* = 0)
	{

		constexpr unsigned int one_val = (1 << level) | lowerBitsVal;
		return Wrapper<ValueSize, false>::mux(
					key.template get<level>(),
					mapping::template map<one_val>(),
					mapping::template map<lowerBitsVal>()
			);
	}

	template<class mapping, unsigned int KeySize, unsigned int ValueSize, template<unsigned int, bool> class Wrapper>
	class TabulatedFunction
	{
		public:
			static inline Wrapper<ValueSize, false> read(Wrapper<KeySize, false> const key)
			{
				static_assert(KeySize <= 8, "Key Size should be smaller or equal to 8 to fit in a single slice");
				return _table_func_impl<mapping, KeySize, ValueSize, 0, 0, Wrapper>(key);
			}

	};

#ifdef VIVADO_BACKEND
	template<class mapping, unsigned int KeySize, unsigned int ValueSize>
	class TabulatedFunction<mapping, KeySize, ValueSize, VivadoWrapper>
	{
		public:
			static inline VivadoWrapper<ValueSize, false> read(VivadoWrapper<KeySize, false> const key)
			{
				#pragma HLS LATENCY min=0 max=0
				#pragma HLS INLINE
				static_assert(KeySize <= 8, "Key Size should be smaller or equal to 8 to fit in a single slice");
				return _table_func_impl<mapping, KeySize, ValueSize, 0, 0, VivadoWrapper>(key);
			}
	};
#endif
}

#endif // TABLE_HPP

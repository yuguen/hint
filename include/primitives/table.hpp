#ifndef TABLE_HPP
#define TABLE_HPP

#include "hint.hpp"
#include "tools/sequence.hpp"

namespace hint {
	template<class mapping, unsigned int KeySize, unsigned int ValueSize, template<unsigned int, bool> class Wrapper>
	class TabulatedFunction;

#ifdef VIVADO_BACKEND
	template<class mapping, unsigned int KeySize, unsigned int ValueSize, unsigned int level, unsigned int lowerBitsVal>
	inline VivadoWrapper<ValueSize, false> _vivado_table_func_impl(
			VivadoWrapper<KeySize, false> const & key,
			typename enable_if<(level < KeySize-1)>::type* = 0)
	{
		#pragma HLS INLINE
		constexpr unsigned int one_val = (1 << level) | lowerBitsVal;
		switch (key.template isSet<level>()) {
			case 1:
				return _vivado_table_func_impl<mapping, KeySize, ValueSize, level + 1, one_val>(key);
			case 0:
				return _vivado_table_func_impl<mapping, KeySize, ValueSize, level + 1, lowerBitsVal>(key);
		}
	}

	template<class mapping, unsigned int KeySize, unsigned int ValueSize, unsigned int level, unsigned int lowerBitsVal>
	inline VivadoWrapper<ValueSize, false> _vivado_table_func_impl(
			VivadoWrapper<KeySize, false> const & key ,
			typename enable_if<(level == (KeySize-1))>::type* = 0)
	{
		#pragma HLS INLINE
		constexpr unsigned int one_val = (1 << level) | lowerBitsVal;
		auto one_ret = mapping::map(one_val);
		auto zero_ret = mapping::map(lowerBitsVal);
		switch (key.template isSet<level>()) {
			case 1:
				return {one_ret};
			case 0:
				return {zero_ret};
		}

	}

	template<class mapping, unsigned int KeySize, unsigned int ValueSize>
	class TabulatedFunction<mapping, KeySize, ValueSize, VivadoWrapper>
	{
		public:
			static inline VivadoWrapper<ValueSize, false> read(VivadoWrapper<KeySize, false> const key)
			{
				#pragma HLS LATENCY min=0 max=0
				#pragma HLS INLINE
				static_assert(KeySize <= 8, "Key Size should be smaller or equal to 8 to fit in a single slice");
				return _vivado_table_func_impl<mapping, KeySize, ValueSize, 0, 0>(key);
			}
	};
#endif
}

#endif // TABLE_HPP

#ifndef SPLITTING_HPP
#define SPLITTING_HPP
#include <array>
#include <type_traits>

#include "tools/static_math.hpp"

using std::enable_if;
using std::array;

namespace hint {

template<unsigned int INPUT_SIZE, unsigned int SLICE_SIZE>
class ArraySplitter {
		static_assert (SLICE_SIZE < INPUT_SIZE, "Strange parameters for ArraySplitter : input size smalle than output size");
	private:
		static constexpr unsigned int NB_SLICES = Static_Ceil_Div<INPUT_SIZE, SLICE_SIZE>::val;
		static constexpr unsigned int EMPTY_BITS = NB_SLICES * SLICE_SIZE - INPUT_SIZE;
	public:
		template<template<unsigned int, bool> class Wrapper>
		using ret_type = array<Wrapper<SLICE_SIZE, false>, NB_SLICES>;

		template<template<unsigned int, bool> class Wrapper>
		using in_type = Wrapper<INPUT_SIZE, false>;

	private:
		static constexpr bool needFillBits(unsigned int lvl) {
			return (lvl == (NB_SLICES - 1)) and (EMPTY_BITS > 0);
		}

		template<unsigned int lvl, template<unsigned int, bool> class Wrapper>
		static inline void _distribute(
				in_type<Wrapper> const & in,
				ret_type<Wrapper>& out,
				typename enable_if<(lvl == 0)>::type* = 0) {
			out[0] = in.template slice<SLICE_SIZE - 1, 0>();
		}

		template<unsigned int lvl, template<unsigned int, bool> class Wrapper>
		static inline void _distribute(
				in_type<Wrapper> const & in,
				ret_type<Wrapper>& out,
				typename enable_if<(lvl > 0) and (needFillBits(lvl))>::type* = 0) {
			constexpr unsigned int up = (lvl+1) * SLICE_SIZE - (1 + EMPTY_BITS);
			constexpr unsigned int down = lvl * SLICE_SIZE;
			out[lvl] = Wrapper<EMPTY_BITS, false>{0}.concatenate(in.template slice<up, down>());
			_distribute<lvl-1>(in, out);
		}

		template<unsigned int lvl, template<unsigned int, bool> class Wrapper>
		static inline void _distribute(
				in_type<Wrapper> const & in,
				ret_type<Wrapper>& out,
				typename enable_if<(lvl > 0) and (!needFillBits(lvl))>::type* = 0) {
			constexpr unsigned int up = (lvl+1) * SLICE_SIZE - 1;
			constexpr unsigned int down = lvl * SLICE_SIZE;
			out[lvl] = in.template slice<up, down>();
			_distribute<lvl-1>(in, out);
		}


	public:
		template<template<unsigned int, bool> class Wrapper>
		static inline void distribute(in_type<Wrapper> const & in, ret_type<Wrapper>& out) {
			_distribute<NB_SLICES - 1>(in, out);
		};
};

}
#endif // SPLITTING_HPP

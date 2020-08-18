#define VIVADO_BACKEND

#include <ap_int.h>
#include "hint.hpp"
#include "primitives/lzoc_shifter.hpp"

using hint::VivadoWrapper;
using hint::LZOC_shift;
constexpr unsigned int WORD_SIZE = 14;

ap_uint<WORD_SIZE + 5> lzocshift(
	VivadoWrapper<WORD_SIZE, false> in, 
	VivadoWrapper<1, false> to_count
)
{
	return LZOC_shift<WORD_SIZE, WORD_SIZE>(in, to_count).unravel();
}

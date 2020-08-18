#define VIVADO_BACKEND

#include <ap_int.h>
#include "hint.hpp"
#include "primitives/backwards.hpp"

using hint::VivadoWrapper;
constexpr unsigned int WORD_SIZE = 64;

ap_uint<WORD_SIZE> reverse(
	VivadoWrapper<WORD_SIZE, false> in
)
{
	return backwards(in).unravel();
}


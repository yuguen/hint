#pragma once

template <size_t W, bool is_signed, typename backend>
struct BackendTypeInfos;

#if defined(AP_INT_BACKEND)
#include "ap_int.h"
template <size_t W, bool is_signed>
struct BackendTypeInfos<W, is_signed, ap_int_base>
{
	using backend_type = ap_int_base<static_cast<int>(W), is_signed>;
};
#endif

#if defined(AC_INT_BACKEND)
#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"

template <size_t W, bool is_signed>
struct BackendTypeInfos<W, is_signed, ac_int>
{
	using backend_type = ac_int<static_cast<int>(W), is_signed>;
};
#endif

#if defined(BITSET_BACKEND)
	#include <bitset>
	using namespace std;
	template <size_t W, bool is_signed>
	struct BackendTypeInfos<W, is_signed, bitset>
	{
		using backend_type = bitset<W>;	
	};
#endif

#ifdef AC_INT_BACKEND
#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"
#endif
#elif defined(AP_INT_BACKEND)
#include "ap_int.h"
#else
#error "At least one of AC_INT_BACKEND and AP_INT_BACKEND must be set"
#endif


template <int W, bool is_signed>
#ifdef AP_INT_BACKEND
struct ac_ap : public ap_int_base<W, is_signed>
{
	using ap_int_base<W, is_signed>::ap_int_base;

	// high excluded, low included
	template <int high, int low>	
	ac_ap<high-low+1, false> from_to(){
		return ac_ap<high-low+1, false>((*this).range(high, low));
	}

	ac_ap<1, false> get(int i){
		return ac_ap<1, false>((*this)[i]);
	}

};



#else // defined(AC_INT_BACKEND)
struct ac_ap : public ac_int<W, is_signed>
{
	using ac_int<W, is_signed>::ac_int;

	// high excluded, low included
	template <int high, int low>	
	ac_ap<high-low+1, false> from_to(){
		return ac_ap<high-low+1, false>((*this).template slc<high-low+1>(low));
	}

	ac_ap<1, false> get(int i){
		return ac_ap<1, false>((*this)[i]);
	}

};
#endif

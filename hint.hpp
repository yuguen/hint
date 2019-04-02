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


template <int W, bool is_signed=false>
#ifdef AP_INT_BACKEND
struct hint_base : public ap_int_base<W, is_signed>
{
	using ap_int_base<W, is_signed>::ap_int_base;

	// high excluded, low included
	hint_base<W, false> slice(int high, int low) const{
		return hint_base<W, false>((*this).range(high, low));
	}

	hint_base<1, false> get(int i) const{
		return hint_base<1, false>((*this)[i]);
	}

	template <int Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, is_signed> concatenate(hint_base<Wrhs, is_signed_rhs> rhs){
		return hint_base<W+Wrhs, is_signed>((*this).concat(rhs));
	}



#else // defined(AC_INT_BACKEND)
struct hint_base : public ac_int<W, is_signed>
{
	// using ac_int<W, is_signed>::ac_int;
	hint_base<W, is_signed>():ac_int<W, is_signed>(){}

	template<typename T>
 	hint_base<W, is_signed>(T val):ac_int<W, is_signed>(val){}



	// high excluded, low included
	hint_base<W, false> slice(const int high, const int low) const{
		const int size = high-low+1;
		return hint_base<W, false>((*this).template slc<W>(low));
	}

	hint_base<1, false> get(int i) const{
		return hint_base<1, false>(this[i]);
	}

	template <int Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, is_signed> concatenate(hint_base<Wrhs, is_signed_rhs> rhs){
		hint_base<W+Wrhs, is_signed> result;
		result.set_slc(0, rhs);
		result.set_slc(Wrhs, (*this));
		return result;
	}

	template<typename T>
	hint_base<1, false> operator [] (T index) const{
		return static_cast< hint_base<W, is_signed> >((*this)[index]);
	}

#endif

	template<typename T>
	hint_base<W, is_signed> operator = (T val) {
		return static_cast< hint_base<W, is_signed> >(val);
	}

	// template<int W1, bool is_signed1>
	// hint_base<W, is_signed> operator=(
	// 	hint_base<W1, is_signed1> const & val
	//    )
	// {
	//  	return hint_base<W, is_signed>(val);  
	// };
	
	void print(){
		if(is_signed){
			fprintf(stderr, "Signed hint: ");
		}
		else{
			fprintf(stderr, "Unsigned hint: ");			
		}
		for(int i = W-1 ; i>=0; i--){
			fprintf(stderr, "%d", (int)(*this).get(i));
		}
		fprintf(stderr, "\n");
	}

};

template <int W>
using hint = hint_base<W, false>;

template <int W>
using hints = hint_base<W, true>;
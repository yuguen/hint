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
class hint_base : public ap_int_base<W, is_signed>
{
public:
	// using ap_int_base<W, is_signed>::ap_int_base;
	hint_base():ap_int_base<W, is_signed>(){}
	
	template <typename T>
	hint_base(T val):ap_int_base<W, is_signed>(val){}

	template<int size>
	hint_base<size, is_signed> slice(int low) const{
		return hint_base<size, is_signed>((*this).range(low+size-1, low));
	}

	template<int W1, bool is_signed1>
	void set_slice(int low, hint_base<W1, is_signed1> value){
		(*this).range(W1+low-1, low) = value;
	}

	hint_base<1, false> get(int i) const{
		return hint_base<1, false>((*this)[i]);
	}

	template <int Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, is_signed> concatenate(hint_base<Wrhs, is_signed_rhs> rhs){
		return hint_base<W+Wrhs, is_signed>((*this).concat(rhs));
	}


#else // defined(AC_INT_BACKEND)
class hint_base : public ac_int<W, is_signed>
{
public:
	using ac_int<W, is_signed>::ac_int;
    hint_base(ac_int<W, is_signed> val):ac_int<W, is_signed>(val){}

	// using ac_int<W, is_signed>::ac_int;
	// hint_base<W, is_signed>():ac_int<W, is_signed>():ac_int(){}

	// template<typename T>
 // 	hint_base<W, is_signed>(T val):ac_int<W, is_signed>():ac_int(val){}



	// high excluded, low included
	// template<int size>
	// hint_base<size, is_signed> slice(int low) const{
	// 	return hint_base<size, is_signed>((*this).template slc<size>(low));
	// }

	template<int size>
	hint_base<size, is_signed> slice(int low) const{
		ac_int<200, false> low_ac = low;
		ac_int<size, is_signed> slc = (*this).template slc<size>(low_ac);
		return hint_base<size, is_signed>(slc);
	}

	// high excluded, low included
	template<int W1, bool is_signed1>
	void set_slice(int low, hint_base<W1, is_signed1> value){
		(*this).template set_slc(hint_base<W1, is_signed1>(low), value);
		// (*this).range(high, low) = value;
	}

	hint_base<1, false> get(int i) const{
		return hint_base<1, false>((*this).template slc<1>(i));
	}

	template <int Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, is_signed> concatenate(hint_base<Wrhs, is_signed_rhs> rhs){
		hint_base<W+Wrhs, is_signed> result;
		result.template set_slc(0, rhs);
		result.template set_slc(Wrhs, (*this));
		return result;
	}

	template<typename T>
	hint_base<1, false> operator [] (T i) const{
		return hint_base<W, is_signed>((*this).template slc<1>(i));
	}

	// hint_base<W, is_signed> operator=(ac_int<W, is_signed> &val) {
	// 	return hint_base<W, is_signed>(val);
	// }


#endif

// const TripleData& rhs

	// template<int W1, bool is_signed1>
	// hint_base<W, is_signed> operator=(
	// 	hint_base<W1, is_signed1> 
	//    )
	// {
	//  	return hint_base<W, is_signed>(val);  
	// };

#ifndef SYNTHESIS	
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
#endif

};

template <int W>
using hint = hint_base<W, false>;

template <int W>
using hints = hint_base<W, true>;
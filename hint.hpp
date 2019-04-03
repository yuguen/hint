#pragma once
#include <type_traits>

using namespace std;

#include "backend/backend_types.hpp"

template <size_t W, typename backend, bool is_signed=false>
struct hint_base : public BackendTypeInfos<W, is_signed, backend>::backend_type
{
	using backend_type = BackendTypeInfos<W, is_signed, backend>::backend_type;

	hint_base(backend_type& val):backend_type{val}{}

	// high and low included
	template<size_t high, size_t low>
	hint_base<high - low + 1, backend, false> slice(
				enable_if<(high>=low)>::type* = 0
			) const;

	template<size_t idx>
	hint_base<1, backend, false> get(
				enable_if<(idx < W)>::type* = 0
			) const;

	template <size_t Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, backend, is_signed> concatenate(
			hint_base<Wrhs, backend, is_signed_rhs> const & rhs
		) const;

	// high excluded, low included
	/*template <int Wrhs, bool is_signed_rhs>	
	hint_base<W+Wrhs, is_signed> concatenate(hint_base<Wrhs, is_signed_rhs> rhs){
		hint_base<W+Wrhs, is_signed> result;
		result.set_slc(0, rhs);
		result.set_slc(Wrhs, (*this));
		return result;
	}*/

	/*
	template<typename T>
	hint_base<W, is_signed> operator = (T val) {
		return static_cast<hint_base<W, , backend, is_signed> >(val);
	}*/

	// template<int W1, bool is_signed1>
	// hint_base<W, is_signed> operator=(
	// 	hint_base<W1, is_signed1> const & val
	//    )
	// {
	//  	return hint_base<W, is_signed>(val);  
	// };
	/*
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
	}*/
};

template <int W, typename backend>
using hint = hint_base<W, backend, false>;

template <int W, typename backend>
using hints = hint_base<W, backend, true>;

#include "backend/backend_impl.in"

#ifndef LZC
#define LZC
// BlueBook LZC Log2N
#include <cstdio>

#include "hint.hpp"
#include "tools/static_math.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter.hpp"
#include "primitives/reverse.hpp"


// template<unsigned int W, bool is_signed, template<unsigned int, bool> class Wrapper>
// Wrapper<Static_Val<W>::_rlog2, false> lzc(Wrapper<W, is_signed> input){
// 	Wrapper<W, is_signed> reversed_input = reverse(input);
// 	enum {P2= Static_Val<W>::_2pow};
// 	enum {L2= Static_Val<W>::_rlog2};
// 	int tmp;
// 	Wrapper<P2,false> upper,lower;
// 	Wrapper<P2,false> mask =  Wrapper<P2,false>::generateSequence(Wrapper<1,false>{1});
// 	Wrapper<P2,false> input_tmp{0};
// 	int idx = 0;
// 	input_tmp = Wrapper<P2,false>{reversed_input.unravel()};
// 	for(int i=0;i<L2;i++){
// 		mask = shifter<true>(mask, Wrapper<Static_Val<P2>::_rlog2,false>{(P2>>1)>>i});
// 		// mask = mask >> ((P2/2)>>i);
// 		upper = Wrapper<P2,false>{0};
// 		lower = Wrapper<P2,false>{0};
// 		upper = shifter<true>(input_tmp, Wrapper<Static_Val<P2>::_rlog2,false>{(P2>>1)>>i});
// 		lower = input_tmp.And(mask);
// 		input_tmp = Wrapper<P2,false>{0};
// 		if(upper.or_reduce().template isSet<0>()){
// 			idx = idx + ((P2>>1) >> i);
// 			input_tmp = upper;
// 		}else{
// 			input_tmp = lower;
// 		}
// 	}

// 	return Wrapper<Static_Val<W>::_rlog2, false>{idx};
// }


template<unsigned int W, bool is_signed, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W>::_rlog2, false> lzc(Wrapper<W, is_signed> input,
	typename std::enable_if< (W>1) >::type* = 0
	,typename std::enable_if< (Static_Val<W>::_rlog2 - Static_Val<Static_Val<Static_Val<(W+1)/2>::_2pow>::_2pow>::_rlog2)!=0>::type* = 0
	){

	cout << "input size: " << W << ": " << to_string(input) << endl;

	enum {P2 = Static_Val<(W+1)/2>::_2pow};
	Wrapper<W-P2, false> upper;
	Wrapper<P2, false> lower;
	Wrapper<Static_Val<W>::_rlog2, false> idx{0};
	Wrapper<Static_Val<W-P2>::_rlog2, false> idxu{0};
	Wrapper<Static_Val<P2>::_rlog2, false> idxl{0};
	Wrapper<Static_Val<W>::_rlog2 - Static_Val<W-P2>::_rlog2, false> padding_idxu{0};
	Wrapper<Static_Val<W>::_rlog2 - Static_Val<P2>::_rlog2, false> padding_idxl{0};
	upper = input.template slice<W-1, P2>();
	lower = input.template slice<P2-1, 0>();
	if(upper.or_reduce().template isSet<0>()){
		cout << "reccursive call on upper" << endl;
		idxu = lzc<W-P2>(upper);
		idx = padding_idxu.concatenate(idxu);
		// .Or(Wrapper<Static_Val<W-P2>::_rlog2, false>{P2});
	}
	else{
		cout << "reccursive call on lower" << endl; 

		idxl = lzc<P2>(lower);
		idx = padding_idxl.concatenate(idxl).Or(Wrapper<Static_Val<W>::_rlog2, false>{W-P2});
	}
	return idx;
}

template<unsigned int W, bool is_signed, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W>::_rlog2, false> lzc(Wrapper<W, is_signed> input,
	typename std::enable_if< (W>1) >::type* = 0,
	typename std::enable_if< (Static_Val<W>::_rlog2 - Static_Val<Static_Val<(W+1)/2>::_2pow>::_rlog2)==0>::type* = 0
	){
	cout << "input size: " << W << ": " << to_string(input) << endl;

	enum {P2 = Static_Val<(W+1)/2>::_2pow};
	Wrapper<W-P2, false> upper;
	Wrapper<P2, false> lower;
	Wrapper<Static_Val<W>::_rlog2, false> idx{0};
	// Wrapper<Static_Val<W-P2>::_rlog2, false> idxu{0};
	Wrapper<Static_Val<P2>::_rlog2, false> idxl{0};
	upper = input.template slice<W-1, P2>();
	lower = input.template slice<P2-1, 0>();
	if(upper.or_reduce().template isSet<0>()){
		cout << "reccursive call on upper" << endl;
		// idxu = lzc<W-P2>(upper);
		// idx = idxu.Or(Wrapper<Static_Val<W-P2>::_rlog2, false>{P2});
		idx = Wrapper<Static_Val<W>::_rlog2, false>{0};
	}
	else{
		cout << "reccursive call on lower" << endl; 
		idxl = lzc<P2>(lower);
		idx = idxl.Or(Wrapper<Static_Val<P2>::_rlog2, false>{W-P2});
	}
	return idx;
}

template<unsigned int W, bool is_signed, template<unsigned int, bool> class Wrapper>
Wrapper<Static_Val<W>::_rlog2, false> lzc(Wrapper<W, is_signed> input,
	typename std::enable_if< (W==1) >::type* = 0){
	return Wrapper<Static_Val<W>::_rlog2, false>{not input.template isSet<0>()};
}


#endif // LZC

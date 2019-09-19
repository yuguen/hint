#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE GMPBackendTestModule

#define WRAPPER GMPWrapper

#define SIZE 28

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/lzoc_shifter.hpp"
#include "primitives/lzoc.hpp"
#include "primitives/shifter_sticky.hpp"

using namespace  std;
using namespace  hint;

BOOST_AUTO_TEST_CASE(testBackend)
{
    // Simple slice that gets a subset of the input 
    // Verification using isSet() 
    WRAPPER<5, false> a{0b01110};
    WRAPPER<3, false> slice_a = a.template slice<4, 2>();

    BOOST_REQUIRE_MESSAGE(slice_a.isSet<0>(), "The slice method failed! (a)");
    BOOST_REQUIRE_MESSAGE(slice_a.isSet<1>(), "The slice method failed! (a)");
    BOOST_REQUIRE_MESSAGE(not(slice_a.isSet<2>()), "The slice method failed! (a)");


    // Slice that gets all the bits of the input
    WRAPPER<2, false> b{0b01};
    WRAPPER<2, false> slice_b = b.template slice<1, 0>();

    BOOST_REQUIRE_MESSAGE(slice_b.isSet<0>(), "The slice method failed! (b)");
    BOOST_REQUIRE_MESSAGE(not(slice_b.isSet<1>()), "The slice method failed! (b)");

    // == operator with slices
    WRAPPER<64, true> c{0b0001110111101011100100101001001001111110101111101001001001001111};
    WRAPPER<60, false> slice_c1 = c.template slice<59, 0>();
    WRAPPER<4, false> slice_c2 = c.template slice<63, 60>();

    WRAPPER<60, false> cmp_slice_c1{0b110111101011100100101001001001111110101111101001001001001111};
    WRAPPER<4, false> cmp_slice_c2{0b0001};
    WRAPPER<4, false> cmp_slice_c3{0b0011};
    
    WRAPPER<1, false> cmp_c1{slice_c1 == cmp_slice_c1};
    WRAPPER<1, false> cmp_c2{slice_c2 == cmp_slice_c2};
    WRAPPER<1, false> cmp_c3{slice_c2 == cmp_slice_c3};
    BOOST_REQUIRE_MESSAGE(cmp_c1.isSet<0>(), "The slice method failed! (c)");
    BOOST_REQUIRE_MESSAGE(cmp_c2.isSet<0>(), "The slice method failed! (c)");
    BOOST_REQUIRE_MESSAGE(not(cmp_c3.isSet<0>()), "The slice method failed! (c)");

    // get() method
    WRAPPER<1, false> get_d1{cmp_slice_c3.get<0>()};
    WRAPPER<1, false> get_d2{cmp_slice_c3.get<1>()};
    WRAPPER<1, false> get_d3{cmp_slice_c3.get<2>()};

    BOOST_REQUIRE_MESSAGE(get_d1.isSet<0>(), "The get method failed! (d)");
    BOOST_REQUIRE_MESSAGE(get_d2.isSet<0>(), "The get method failed! (d)");
    BOOST_REQUIRE_MESSAGE(not(get_d3.isSet<0>()), "The get method failed! (d)");

    // concatenate method
    // unsigned
    WRAPPER<29, false> e1{0b11011110101110010010100100100};
    WRAPPER<17, false> e2{0b00010011101110111};
    WRAPPER<46, false> concat_e1_e2{e1.concatenate(e2)};
    WRAPPER<46, false> expected_e1_e2{0b1101111010111001001010010010000010011101110111};
    WRAPPER<1, false> cmp_e_unsigned{expected_e1_e2 == concat_e1_e2};

    BOOST_REQUIRE_MESSAGE(cmp_e_unsigned.isSet<0>(), "The concatenate method failed! (e)");

    // signed
    WRAPPER<29, true> e3{0b11011110101110010010100100100};
    WRAPPER<17, false> e4{0b00010011101110111};
    WRAPPER<46, false> concat_e3_e4{e3.concatenate(e4)};
    WRAPPER<46, false> expected_e3_e4{0b1101111010111001001010010010000010011101110111};
    WRAPPER<1, false> cmp_e_signed{expected_e3_e4 == concat_e3_e4};

    BOOST_REQUIRE_MESSAGE(cmp_e_signed.isSet<0>(), "The concatenate method failed! (e)");


    // addWithCarry method
    // signed
    WRAPPER<14, true> f1{0b11111111111111};
    WRAPPER<14, true> f2{0b11111111111111};
    WRAPPER<1, false> carry_f1{0b1};

    WRAPPER<15, true> sum_f1_f2 = f1.addWithCarry(f2, carry_f1);
    WRAPPER<15, true> expected_sum_f1_f2{0b111111111111111};
    WRAPPER<1, false> cmp_sum_f1_f2{expected_sum_f1_f2 == sum_f1_f2};
    BOOST_REQUIRE_MESSAGE(cmp_sum_f1_f2.isSet<0>(), "The addWithCarry method failed! (f)");

    // unsigned
    WRAPPER<14, false> f3{0b10010010110100};
    WRAPPER<14, false> f4{0b11010010011101};
    WRAPPER<1, false> carry_f3{0b1};

    WRAPPER<15, false> sum_f3_f4 = f3.addWithCarry(f4, carry_f3);
    WRAPPER<15, false> expected_sum_f3_f4{0b101100101010010};
    WRAPPER<1, false> cmp_sum_f3_f4{expected_sum_f3_f4 == sum_f3_f4};
    BOOST_REQUIRE_MESSAGE(cmp_sum_f3_f4.isSet<0>(), "The addWithCarry method failed! (f)");

    // modularAdd method
    // unsigned
    WRAPPER<14, false> g1{0b10010010110100};
    WRAPPER<14, false> g2{0b11010010011101};

    WRAPPER<14, false> modular_sum_g1_g2{g1.modularAdd(g2)};
    WRAPPER<14, false> expected_sum_g1_g2{0b01100101010001};

    WRAPPER<1, false> cmp_sum_g1_g2{expected_sum_g1_g2 == modular_sum_g1_g2};
    BOOST_REQUIRE_MESSAGE(cmp_sum_g1_g2.isSet<0>(), "The modularAdd method failed! (g)");

    // signed
    WRAPPER<14, true> g3{0b00110100101001};
    WRAPPER<14, true> g4{0b00111011001100};

    WRAPPER<14, false> modular_sum_g3_g4{g3.modularAdd(g4)};
    WRAPPER<14, false> expected_sum_g3_g4{0b1101111110101};

    WRAPPER<1, false> cmp_sum_g3_g4{expected_sum_g3_g4 == modular_sum_g3_g4};
    BOOST_REQUIRE_MESSAGE(cmp_sum_g3_g4.isSet<0>(), "The modularAdd method failed! (g)");

    // generateSequence method
    WRAPPER<14, false> h1{WRAPPER<14, false>::generateSequence(WRAPPER<1, false>(1))};
    WRAPPER<14, false> expected_h1{0b11111111111111};
    WRAPPER<1, false> cmp_h1{expected_h1 == h1};
    BOOST_REQUIRE_MESSAGE(cmp_h1.isSet<0>(), "The generateSequence method failed! (h)");

    // generateSequence method
    WRAPPER<14, false> h2{WRAPPER<14, false>::generateSequence(WRAPPER<1, false>(0))};
    WRAPPER<14, false> expected_h2{0b0};
    WRAPPER<1, false> cmp_h2{expected_h2 == h2};
    BOOST_REQUIRE_MESSAGE(cmp_h2.isSet<0>(), "The generateSequence method failed! (h)");
}

BOOST_AUTO_TEST_CASE(testLzocShifterAndShifterGMP)
{
	GMPWrapper<SIZE, false> currentValue{0};
	GMPWrapper<SIZE+1, false> shifted_with_sticky;
	GMPWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> computed_lzoc_shift;
	GMPWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> expected_lzoc_shift;
	GMPWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc;
	GMPWrapper<SIZE, false> expected_shift;
	GMPWrapper<1, false> cmp;
	for(int i=0; i<SIZE; i++){
		auto back = backwards(currentValue);
		shifted_with_sticky = shifter_sticky(back, GMPWrapper<1, false>{1}, GMPWrapper<1, false>{1});
		//cerr << to_string(shifted_with_sticky) << endl;
		currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
		//cerr << to_string(currentValue) << endl;
		computed_lzoc_shift = LZOC_shift<SIZE, SIZE>(currentValue, GMPWrapper<1, false>{0}, GMPWrapper<1, false>{0});
		//cerr << to_string(computed_lzoc_shift) << endl;
		expected_lzoc = GMPWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-i-1};
		//cerr << to_string(expected_lzoc) << endl;
		expected_shift = backwards(currentValue);
		//cerr << to_string(expected_shift) << endl;
		expected_lzoc_shift = expected_lzoc.concatenate(expected_shift);
		//cerr << to_string(expected_lzoc_shift) << endl;
		cmp = GMPWrapper<1, false>{expected_lzoc_shift == computed_lzoc_shift};
		BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Intel The combined test of the shifter_sticky and the lzoc_shifter failed !");
	}
}

BOOST_AUTO_TEST_CASE(testLzocGMP)
{
	GMPWrapper<SIZE, false> currentValue{0};
	GMPWrapper<SIZE+1, false> shifted_with_sticky;
	GMPWrapper<Static_Val<SIZE+1>::_clog2, false> computed_lzoc;
	GMPWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc;
	GMPWrapper<1, false> cmp;
	for(int i=0; i<SIZE; i++){
		shifted_with_sticky = shifter_sticky(backwards(currentValue), GMPWrapper<1, false>{1}, GMPWrapper<1, false>{1});
		//cerr << to_string(shifted_with_sticky) << endl;
		currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
		//cerr << to_string(currentValue) << endl;
		computed_lzoc = lzoc_wrapper(currentValue, GMPWrapper<1, false>{0});
		//cerr << to_string(computed_lzoc) << endl;
		expected_lzoc = GMPWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-i-1};
		//cerr << to_string(expected_lzoc) << endl;
		cmp = GMPWrapper<1, false>{expected_lzoc == computed_lzoc};
		BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Test of lzoc failed !!! i = "<< i);
	}
	// fprintf(stderr, "The tests passed\n");
}

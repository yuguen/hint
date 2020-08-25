#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE PrimitiveTestModule

#define AP_INT_MAX_W 1050

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/lzoc_shifter.hpp"
#include "primitives/lzoc.hpp"
#include "primitives/shifter_sticky.hpp"
#include "primitives/lossless_shifter.hpp"
#include "primitives/zero_one_normaliser.hpp"


using namespace  std;
using namespace hint;

#define SIZE 28

template<template<unsigned int, bool> class Wrapper>
bool test_lm_indicator(void)
{
    constexpr unsigned int WIDTH = 9;
    Wrapper<WIDTH, false> zero{0};
    auto zero_indic = leftmost_indicator(zero);
    if (zero_indic.unravel() != 1) {
        return false;
    }
    for (unsigned int shift = 0; shift < WIDTH ; shift++) {
        unsigned int start = 1 << shift;
        auto expected_res = start << 1;
        for(unsigned int curoff = 0 ; curoff < (1 << shift) ;  curoff += 1) {
            Wrapper<WIDTH, false> input {start + curoff};
            auto res = leftmost_indicator(input);
            if (res.unravel() != expected_res) {
                return false;
            }
        }
    }
    return true;
}

template<template<unsigned int, bool> class Wrapper>
bool test_fast_lzc(void)
{
    constexpr unsigned int WIDTH = 9;
    Wrapper<WIDTH, false> zero{0};
    auto zero_indic = fast_lzc(zero);
    if (zero_indic.unravel() != WIDTH) {
        return false;
    }
    for (unsigned int shift = 0; shift < WIDTH ; shift++) {
        unsigned int start = 1 << shift;
        auto expected_res = WIDTH - 1 - shift;
        for(unsigned int curoff = 0 ; curoff < (1 << shift) ;  curoff += 1) {
            Wrapper<WIDTH, false> input {start + curoff};
            auto res = fast_lzc(input);
            if (res.unravel() != expected_res) {
                return false;
            }
        }
    }
    return true;
}

#if defined(VIVADO_BACKEND)
BOOST_AUTO_TEST_CASE(testLossLessShifter)
{
    VivadoWrapper<1, false> currentValue{1};
    VivadoWrapper<1+(1<<2)-1, false> expected{1};
    VivadoWrapper<2, false> shift_value{0};
    VivadoWrapper<1, false> cmp  = VivadoWrapper<1, false>{expected == lossless_shifter<false>(currentValue, shift_value)};
    BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Vivado The combined test of the shifter_sticky and the lzoc_shifter failed !");
}

BOOST_AUTO_TEST_CASE(TestLeftmostIndicatorVivado)
{
    BOOST_REQUIRE(test_lm_indicator<VivadoWrapper>());
}

BOOST_AUTO_TEST_CASE(TestFastLZCVivado)
{
    BOOST_REQUIRE(test_fast_lzc<VivadoWrapper>());
}

BOOST_AUTO_TEST_CASE(testLzocShifterAndShifterVivado)
{
	VivadoWrapper<SIZE, false> currentValue{0};
	VivadoWrapper<SIZE+1, false> shifted_with_sticky;
	VivadoWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> computed_lzoc_shift;
	VivadoWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> expected_lzoc_shift;
    VivadoWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc;
    VivadoWrapper<SIZE, false> expected_shift;
	VivadoWrapper<1, false> cmp;
    for(int i=0; i<SIZE; i++){
        shifted_with_sticky = shifter_sticky(backwards(currentValue), VivadoWrapper<1, false>{1}, VivadoWrapper<1, false>{1});
//		cerr << to_string(shifted_with_sticky) << endl;
        currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
//		cerr << to_string(currentValue) << endl;
		computed_lzoc_shift = LZOC_shift<SIZE, SIZE>(currentValue, VivadoWrapper<1, false>{0}, VivadoWrapper<1, false>{0});
//		cerr << to_string(computed_lzoc_shift) << endl;
        expected_lzoc = VivadoWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-1-i};
//		cerr << to_string(expected_lzoc) << endl;
        expected_shift = backwards(currentValue);
//      cerr << to_string(expected_shift) << endl;
        expected_lzoc_shift = expected_lzoc.concatenate(expected_shift);
//      cerr << to_string(expected_lzoc_shift) << endl;
		cmp = VivadoWrapper<1, false>{expected_lzoc_shift == computed_lzoc_shift};
		BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Vivado The combined test of the shifter_sticky and the lzoc_shifter failed !");
	}
}

BOOST_AUTO_TEST_CASE(testLzocVivado)
{
    VivadoWrapper<SIZE, false> currentValue{0};
    VivadoWrapper<SIZE+1, false> shifted_with_sticky;
    VivadoWrapper<Static_Val<SIZE+1>::_clog2, false> computed_lzoc;
    VivadoWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc;
    VivadoWrapper<1, false> cmp;
    for(int i=0; i<SIZE; i++){
        shifted_with_sticky = shifter_sticky(backwards(currentValue), VivadoWrapper<1, false>{1}, VivadoWrapper<1, false>{1});
        // cerr << to_string(shifted_with_sticky) << endl;
        currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
        // cerr << to_string(currentValue) << endl;
        computed_lzoc = lzoc(currentValue, VivadoWrapper<1, false>{0});
        // cerr << to_string(computed_lzoc) << endl;
        expected_lzoc = VivadoWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-i-1};
        // cerr << to_string(expected_lzoc) << endl;
        cmp = VivadoWrapper<1, false>{expected_lzoc == computed_lzoc};
        BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Test of lzoc falied !!! i = "<< i);
    }
    // fprintf(stderr, "The tests passed\n");
}

#endif

#if defined(INTEL_BACKEND)
BOOST_AUTO_TEST_CASE(testLzocShifterAndShifterIntel)
{
	IntelWrapper<SIZE, false> currentValue{0};
	IntelWrapper<SIZE+1, false> shifted_with_sticky;
	IntelWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> computed_lzoc_shift;
	IntelWrapper<Static_Val<SIZE+1>::_clog2 + SIZE, false> expected_lzoc_shift;
	IntelWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc; 
	IntelWrapper<SIZE, false> expected_shift; 
	IntelWrapper<1, false> cmp;
    for(int i=0; i<SIZE; i++){
        shifted_with_sticky = shifter_sticky(backwards(currentValue), IntelWrapper<1, false>{1}, IntelWrapper<1, false>{1});
        // cerr << to_string(shifted_with_sticky) << endl;
        currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
		// cerr << to_string(currentValue) << endl;
		computed_lzoc_shift = LZOC_shift<SIZE, SIZE>(currentValue, IntelWrapper<1, false>{0}, IntelWrapper<1, false>{0});
		// cerr << to_string(computed_lzoc_shift) << endl;
		expected_lzoc = IntelWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-i-1};
		// cerr << to_string(expected_lzoc) << endl;
		expected_shift = backwards(currentValue);
		// cerr << to_string(expected_shift) << endl;
		expected_lzoc_shift = expected_lzoc.concatenate(expected_shift);
		// cerr << to_string(expected_lzoc_shift) << endl;
		cmp = IntelWrapper<1, false>{expected_lzoc_shift == computed_lzoc_shift};
		BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Intel The combined test of the shifter_sticky and the lzoc_shifter failed !");
	}
}
BOOST_AUTO_TEST_CASE(testLzocIntel)
{
    IntelWrapper<SIZE, false> currentValue{0};
    IntelWrapper<SIZE+1, false> shifted_with_sticky;
    IntelWrapper<Static_Val<SIZE+1>::_clog2, false> computed_lzoc;
    IntelWrapper<Static_Val<SIZE+1>::_clog2, false> expected_lzoc;
    IntelWrapper<1, false> cmp;
    for(int i=0; i<SIZE; i++){
        shifted_with_sticky = shifter_sticky(backwards(currentValue), IntelWrapper<1, false>{1}, IntelWrapper<1, false>{1});
        // cerr << to_string(shifted_with_sticky) << endl;
        currentValue = backwards(shifted_with_sticky.slice<SIZE, 1>());
        // cerr << to_string(currentValue) << endl;
        computed_lzoc = lzoc_wrapper(currentValue, IntelWrapper<1, false>{0});
        // cerr << to_string(computed_lzoc) << endl;
        expected_lzoc = IntelWrapper<Static_Val<SIZE+1>::_clog2, false>{SIZE-i-1};
        // cerr << to_string(expected_lzoc) << endl;
        cmp = IntelWrapper<1, false>{expected_lzoc == computed_lzoc};
        BOOST_REQUIRE_MESSAGE(cmp.isSet<0>(), "Test of lzoc falied !!! i = "<< i);
    }
    // fprintf(stderr, "The tests passed\n");
}
#endif

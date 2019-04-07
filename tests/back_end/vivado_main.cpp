#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE VivadoBackendTestModule

#define WRAPPER VivadoWrapper

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/lzoc_shifter.hpp"

using namespace  std;

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

    BOOST_REQUIRE_MESSAGE(get_d1.isSet<0>(), "The slice method failed! (d)");
    BOOST_REQUIRE_MESSAGE(get_d2.isSet<0>(), "The slice method failed! (d)");
    BOOST_REQUIRE_MESSAGE(not(get_d3.isSet<0>()), "The slice method failed! (d)");

    // concatenate method
    // unsigned
    WRAPPER<29, false> e1{0b11011110101110010010100100100};
    WRAPPER<17, false> e2{0b00010011101110111};
    WRAPPER<46, false> concat_e1_e2{e1.concatenate(e2)};
    WRAPPER<46, false> expected_e1_e2{0b1101111010111001001010010010000010011101110111};
    WRAPPER<1, false> cmp_e_unsigned{expected_e1_e2 == concat_e1_e2};

    BOOST_REQUIRE_MESSAGE(cmp_e_unsigned.isSet<0>(), "The slice method failed! (e)");

    // signed
    WRAPPER<29, true> e3{0b11011110101110010010100100100};
    WRAPPER<17, false> e4{0b00010011101110111};
    WRAPPER<46, false> concat_e3_e4{e3.concatenate(e4)};
    WRAPPER<46, false> expected_e3_e4{0b1101111010111001001010010010000010011101110111};
    WRAPPER<1, false> cmp_e_signed{expected_e3_e4 == concat_e3_e4};

    BOOST_REQUIRE_MESSAGE(cmp_e_signed.isSet<0>(), "The slice method failed! (e)");


    // addWithCarry method
    // signed
    WRAPPER<14, true> f1{0b11111111111111};
    WRAPPER<14, true> f2{0b11111111111111};
    WRAPPER<1, false> carry_f1{0b1};

    WRAPPER<15, true> sum_f1_f2 = f1.addWithCarry(f2, carry_f1);
    WRAPPER<15, true> expected_sum_f1_f2{0b111111111111111};
    WRAPPER<1, false> cmp_sum_f1_f2{expected_sum_f1_f2 == sum_f1_f2};
    BOOST_REQUIRE_MESSAGE(cmp_sum_f1_f2.isSet<0>(), "The slice method failed! (f)");

    // unsigned
    WRAPPER<14, false> f3{0b10010010110100};
    WRAPPER<14, false> f4{0b11010010011101};
    WRAPPER<1, false> carry_f3{0b1};

    WRAPPER<15, false> sum_f3_f4 = f3.addWithCarry(f4, carry_f3);
    WRAPPER<15, false> expected_sum_f3_f4{0b101100101010010};
    WRAPPER<1, false> cmp_sum_f3_f4{expected_sum_f3_f4 == sum_f3_f4};
    BOOST_REQUIRE_MESSAGE(cmp_sum_f3_f4.isSet<0>(), "The slice method failed! (f)");


    // inline wrapper<W+1, is_signed> addWithCarry(wrapper<W, is_signed> const & op2, wrapper<1, false> const & cin)
    // {
    //     return static_cast<wrapper<W, is_signed> const *>(this)->perform_addc(
    //         op2, cin
    //     );
    // }


}


// BOOST_AUTO_TEST_CASE(TMP)
// {
//     WRAPPER<32, false> k{17};
//     string s = to_string(k);
//     cerr << s << endl;
//     auto t = lzoc_shifter<5, 4>(k, WRAPPER<1, false>{0}, WRAPPER<1, false>{0});
//     s = to_string(t);
//     cerr << s << endl;
//     BOOST_REQUIRE_MESSAGE(false, "THe test failed !");
// }

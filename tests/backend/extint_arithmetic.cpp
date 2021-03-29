#include <boost/test/unit_test.hpp>

#include "hint.hpp"

using hint::ExtIntWrapper;

#define STATIC_COMPARE(ref, val, msg)                                          \
  static_assert((ref == val).unravel(), msg);


BOOST_AUTO_TEST_CASE(TestCompOP) {
  constexpr unsigned int width = 9;
  using TestType = ExtIntWrapper<width, false>;
  constexpr unsigned int upper_limit = 1 << width;
  for (unsigned int i = 0; i < upper_limit; ++i) {
    TestType wrapped_i{static_cast<unsigned _ExtInt(width)>(i)};
    for (unsigned int j = 0; j < i; ++j) {
      TestType wrapped_j{static_cast<unsigned _ExtInt(width)>(j)};
      BOOST_REQUIRE((wrapped_i >= wrapped_j).unravel());
      BOOST_REQUIRE((wrapped_i > wrapped_j).unravel());
      BOOST_REQUIRE(!(wrapped_j >= wrapped_i).unravel());
      BOOST_REQUIRE(!(wrapped_j > wrapped_i).unravel());
      BOOST_REQUIRE(!(wrapped_i <= wrapped_j).unravel());
      BOOST_REQUIRE(!(wrapped_i < wrapped_j).unravel());
      BOOST_REQUIRE((wrapped_j <= wrapped_i).unravel());
      BOOST_REQUIRE((wrapped_j < wrapped_i).unravel());
      BOOST_REQUIRE((wrapped_j != wrapped_i).unravel());
    }
    BOOST_REQUIRE((wrapped_i == wrapped_i).unravel());
    BOOST_REQUIRE((wrapped_i >= wrapped_i).unravel());
    BOOST_REQUIRE((wrapped_i <= wrapped_i).unravel());
    BOOST_REQUIRE(!(wrapped_i > wrapped_i).unravel());
    BOOST_REQUIRE(!(wrapped_i < wrapped_i).unravel());
    BOOST_REQUIRE(!(wrapped_i != wrapped_i).unravel());
  }
}

BOOST_AUTO_TEST_CASE(TestCompOPWidth1) {
  constexpr unsigned int width = 1;
  using TestType = ExtIntWrapper<width, false>;
  constexpr TestType one{1}, zero{0};

  // ==
  static_assert((one == one).unravel(), "W=1 equality");
  static_assert(!(one == zero).unravel(), "W=1 equality");
  static_assert(!(zero == one).unravel(), "W=1 equality");
  static_assert((zero == zero).unravel(), "W=1 equality");

  // !=
  static_assert(!(one != one).unravel(), "W=1 inequality");
  static_assert((one != zero).unravel(), "W=1 inequality");
  static_assert((zero != one).unravel(), "W=1 inequality");
  static_assert(!(zero != zero).unravel(), "W=1 inequality");

  // >=
  static_assert((one >= one).unravel(), "W=1 greater or equal");
  static_assert((one >= zero).unravel(), "W=1 greater or equal");
  static_assert(!(zero >= one).unravel(), "W=1 greater or equal");
  static_assert((zero >= zero).unravel(), "W=1 greater or equal");

  // >
  static_assert(!(one > one).unravel(), "W=1 strictly greater");
  static_assert((one > zero).unravel(), "W=1 strictly greater");
  static_assert(!(zero > one).unravel(), "W=1 strictly greater");
  static_assert(!(zero > zero).unravel(), "W=1 strictly greater");

  // <
  static_assert(!(one < one).unravel(), "W=1 strictly smaller");
  static_assert(!(one < zero).unravel(), "W=1 strictly smaller");
  static_assert((zero < one).unravel(), "W=1 strictly smaller");
  static_assert(!(zero < zero).unravel(), "W=1 strictly smaller");

  // <=
  static_assert((one <= one).unravel(), "W=1 smaller or equal");
  static_assert(!(one <= zero).unravel(), "W=1 smaller or equal");
  static_assert((zero <= one).unravel(), "W=1 smaller or equal");
  static_assert((zero <= zero).unravel(), "W=1 smaller or equal");
}

BOOST_AUTO_TEST_CASE(TestModularAddSubUS) {
  using TestType = ExtIntWrapper<5, false>;
  constexpr TestType all_one{0x1F}, zero{0}, small_of{0x1E}, one{1};

  // Modular add
  STATIC_COMPARE(small_of, all_one.modularAdd(all_one), "modularAdd() error")
  STATIC_COMPARE(all_one, all_one.modularAdd(zero), "modularAdd() error")
  STATIC_COMPARE(zero, all_one.modularAdd(one), "modularAdd() error")
  STATIC_COMPARE(TestType{0b11101}, all_one.modularAdd(small_of),
                 "modularAdd() error")
  STATIC_COMPARE(zero, zero.modularAdd(zero), "modularAdd() error")
  STATIC_COMPARE(small_of, zero.modularAdd(small_of), "modularAdd() error")
  STATIC_COMPARE(one, zero.modularAdd(one), "modularAdd() error")
  STATIC_COMPARE(TestType{0b11100}, small_of.modularAdd(small_of),
                 "modularAdd() error")
  STATIC_COMPARE(all_one, small_of.modularAdd(one), "modularAdd() error")
  STATIC_COMPARE(TestType{2}, one.modularAdd(one), "modularAdd() error")

  // Modular sub
  STATIC_COMPARE(zero, all_one.modularSub(all_one), "modularSub() error")
  STATIC_COMPARE(all_one, all_one.modularSub(zero), "modularSub() error")
  STATIC_COMPARE(small_of, all_one.modularSub(one), "modularSub() error")
  STATIC_COMPARE(one, all_one.modularSub(small_of), "modularSub() error")

  STATIC_COMPARE(one, zero.modularSub(all_one), "modularSub() error")
  STATIC_COMPARE(zero, zero.modularSub(zero), "modularSub() error")
  STATIC_COMPARE(TestType{2}, zero.modularSub(small_of), "modularSub() error")
  STATIC_COMPARE(all_one, zero.modularSub(one), "modularSub() error")

  STATIC_COMPARE(all_one, small_of.modularSub(all_one), "modularSub() error")
  STATIC_COMPARE(TestType{0b11101}, small_of.modularSub(one),
                 "modularSub() error")
  STATIC_COMPARE(zero, small_of.modularSub(small_of), "modularSub() error")
  STATIC_COMPARE(small_of, small_of.modularSub(zero), "modularSub() error")

  STATIC_COMPARE(TestType{2}, one.modularSub(all_one), "modularSub() error")
  STATIC_COMPARE(one, one.modularSub(zero), "modularSub() error")
  STATIC_COMPARE(TestType{3}, one.modularSub(small_of), "modularSub() error")
  STATIC_COMPARE(zero, one.modularSub(one), "modularSub() error")
}

BOOST_AUTO_TEST_CASE(TestAddCarry) {
  using TestType = ExtIntWrapper<5, false>;
  using ExtResType = ExtIntWrapper<6, false>;
  using CarryType = ExtIntWrapper<1, false>;
  constexpr TestType all_one{0x1F}, zero{0}, small_of{0x1E}, one{1};
  constexpr CarryType cin{1}, nocin{0};

  // Add with carry
#define HINT_EXTINT_TEST_AC(op1, op2, resnocarry, rescarry)                    \
  STATIC_COMPARE(ExtResType{resnocarry}, op1.addWithCarry(op2, nocin),         \
                 "addWithCarry() Error")                                       \
  STATIC_COMPARE(ExtResType{rescarry}, op1.addWithCarry(op2, cin),             \
                 "addWithCarry() Error")

  HINT_EXTINT_TEST_AC(all_one, all_one, 0x3E, 0X3F)
  HINT_EXTINT_TEST_AC(all_one, zero, 0x1F, 0X20)
  HINT_EXTINT_TEST_AC(all_one, small_of, 0x3D, 0x3E)
  HINT_EXTINT_TEST_AC(all_one, one, 0x20, 0x21)

  HINT_EXTINT_TEST_AC(zero, zero, 0x00, 0X01)
  HINT_EXTINT_TEST_AC(zero, small_of, 0x1E, 0x1F)
  HINT_EXTINT_TEST_AC(zero, one, 0x01, 0x02)

  HINT_EXTINT_TEST_AC(small_of, small_of, 0x3C, 0x3D)
  HINT_EXTINT_TEST_AC(small_of, one, 0x1F, 0x20)

  HINT_EXTINT_TEST_AC(one, one, 0x2, 0x3)
#undef HINT_EXTINT_TEST_AC
}

BOOST_AUTO_TEST_CASE(TestAddCarryWidth1US) {
  constexpr ExtIntWrapper<1, false> one{1}, zero{false};
  using ExtResType = ExtIntWrapper<2, false>;
#define HINT_EXTINT_TEST_AC(op1, op2, resnocarry, rescarry)                    \
  STATIC_COMPARE(ExtResType{resnocarry}, op1.addWithCarry(op2, zero),          \
                 "addWithCarry() Error")                                       \
  STATIC_COMPARE(ExtResType{rescarry}, op1.addWithCarry(op2, one),             \
                 "addWithCarry() Error")

  HINT_EXTINT_TEST_AC(one, one, 2, 3)
  HINT_EXTINT_TEST_AC(one, zero, 1, 2)
  HINT_EXTINT_TEST_AC(zero, zero, 0, 1)

#undef HINT_EXTINT_TEST_AC
}

BOOST_AUTO_TEST_CASE(TestAddCarryWidth1Signed) {
  constexpr ExtIntWrapper<1, true> one{1}, zero{false};
  using ExtResType = ExtIntWrapper<2, true>;
  constexpr ExtIntWrapper<1, false> cin{1}, nocin{0};
#define HINT_EXTINT_TEST_AC(op1, op2, resnocarry, rescarry)                    \
  STATIC_COMPARE(ExtResType{resnocarry}, op1.addWithCarry(op2, nocin),          \
                 "addWithCarry() Error")                                       \
  STATIC_COMPARE(ExtResType{rescarry}, op1.addWithCarry(op2, cin),             \
                 "addWithCarry() Error")

  HINT_EXTINT_TEST_AC(one, one, -2, -1)
  HINT_EXTINT_TEST_AC(one, zero, -1, 0)
  HINT_EXTINT_TEST_AC(zero, zero, 0, 1)

#undef HINT_EXTINT_TEST_AC
}
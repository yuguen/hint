#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "backend/extint_impl.ipp"
#include "hint.hpp"

using hint::ExtIntWrapper;

BOOST_AUTO_TEST_CASE(TestInvert) {
  constexpr ExtIntWrapper<7, false> a{0}, b{0b1111111}, c{0b1010101},
      d{0b0101010};
  static_assert((a.invert() == b).unravel(), "Invert error");
  static_assert((b.invert() == a).unravel(), "Invert error");
  static_assert((c.invert() == d).unravel(), "Invert error");
  static_assert((d.invert() == c).unravel(), "Invert error");
}

BOOST_AUTO_TEST_CASE(TestInvertWidth_1) {
  constexpr ExtIntWrapper<1, false> one{1}, zero{0};
  static_assert((one.invert() == zero).unravel(), "One inversion error");
  static_assert((zero.invert() == one).unravel(), "Zero inversion error");
}

BOOST_AUTO_TEST_CASE(TestBinaryLogic) {
  using TestType = ExtIntWrapper<4, false>;
  constexpr TestType halffirst{0b1100}, halfmiddle{0b0110};
  static_assert(
      ((halffirst.bitwise_and(halfmiddle)) == TestType{0b0100}).unravel(),
      "bitwise_and() error");
  static_assert(
      ((halffirst.bitwise_or(halfmiddle)) == TestType{0b1110}).unravel(),
      "bitwise_or() error");
  static_assert(
      ((halffirst.bitwise_xor(halfmiddle)) == TestType{0b1010}).unravel(),
      "bitwise_xor() error");

  static_assert(((halffirst & halfmiddle) == TestType{0b0100}).unravel(),
                "operator&() error");
  static_assert(((halffirst | halfmiddle) == TestType{0b1110}).unravel(),
                "operator|() error");
  static_assert(((halffirst ^ halfmiddle) == TestType{0b1010}).unravel(),
                "operator^() error");
}

BOOST_AUTO_TEST_CASE(TestBinaryLogicWidth1) {
  using TestType = ExtIntWrapper<1, false>;
  constexpr TestType one{0b1}, zero{0b0};

  // bitwise_and()
  static_assert(((one.bitwise_and(one)) == one).unravel(),
                "bitwise_and() error");
  static_assert(((one.bitwise_and(zero)) == zero).unravel(),
                "bitwise_and() error");
  static_assert(((zero.bitwise_and(zero)) == zero).unravel(),
                "bitwise_and() error");
  static_assert(((zero.bitwise_and(one)) == zero).unravel(),
                "bitwise_and() error");

  // bitwise_or()
  static_assert(((one.bitwise_or(one)) == one).unravel(), "bitwise_or() error");
  static_assert(((one.bitwise_or(zero)) == one).unravel(),
                "bitwise_or() error");
  static_assert(((zero.bitwise_or(zero)) == zero).unravel(),
                "bitwise_or() error");
  static_assert(((zero.bitwise_or(one)) == one).unravel(),
                "bitwise_or() error");

  // bitwise_xor()
  static_assert(((one.bitwise_xor(one)) == zero).unravel(),
                "bitwise_xor() error");
  static_assert(((one.bitwise_xor(zero)) == one).unravel(),
                "bitwise_xor() error");
  static_assert(((zero.bitwise_xor(zero)) == zero).unravel(),
                "bitwise_xor() error");
  static_assert(((zero.bitwise_xor(one)) == one).unravel(),
                "bitwise_xor() error");

  // operator&()
  static_assert(((one & one) == one).unravel(), "operator&() error");
  static_assert(((one & zero) == zero).unravel(), "operator&() error");
  static_assert(((zero & zero) == zero).unravel(), "operator&() error");
  static_assert(((zero & one) == zero).unravel(), "operator&() error");

  // operator|()
  static_assert(((one | one) == one).unravel(), "operator|() error");
  static_assert(((one | zero) == one).unravel(), "operator|() error");
  static_assert(((zero | zero) == zero).unravel(), "operator|() error");
  static_assert(((zero | one) == one).unravel(), "operator|() error");

  // operator^()
  static_assert(((one ^ one) == zero).unravel(), "operator^() error");
  static_assert(((one ^ zero) == one).unravel(), "operator^() error");
  static_assert(((zero ^ zero) == zero).unravel(), "operator^() error");
  static_assert(((zero ^ one) == one).unravel(), "operator^() error");
}

BOOST_AUTO_TEST_CASE(Backward) {
  using TestTypeEven = ExtIntWrapper<7, false>;
  constexpr TestTypeEven a{0b11101101}, b{0b10110111};
  static_assert((a.backwards()==b).unravel(), "Inversion error");
}
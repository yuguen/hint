#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExtIntBackendTestModule

#include <array>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "hint.hpp"

using hint::ExtIntWrapper;
using std::array;
using std::cout; 
using std::endl;

BOOST_AUTO_TEST_CASE(TestInstantiation) {
  constexpr ExtIntWrapper<12, false> test{};
  constexpr unsigned _ExtInt(12) zero{0};
  static_assert(test.unravel() == zero, "Error with default general Ctor");
}

BOOST_AUTO_TEST_CASE(TestInstantiationWidth1) {
  constexpr ExtIntWrapper<1, false> test{};
  static_assert(!test.unravel(), "Error with default w=1 Ctor");
}

BOOST_AUTO_TEST_CASE(TestNonEmptyCtor) {
  ExtIntWrapper<12, false> test{0b111111111111};
  BOOST_REQUIRE_EQUAL(static_cast<int>(test.unravel()), 0b111111111111);
};

BOOST_AUTO_TEST_CASE(TestEqualityOp) {
  for (unsigned int i : {0, 12, 0b111111, 17, 1}) {
    for (unsigned int j : {0, 12, 0b111111, 17, 1}) {
      ExtIntWrapper<6, false> wrappedI{static_cast<unsigned _ExtInt(6)>(i)},
          wrappedJ{static_cast<unsigned _ExtInt(6)>(j)};
      auto wrappedEqual = (wrappedI == wrappedJ);
      auto inEqual = (i == j);
      BOOST_REQUIRE_EQUAL(wrappedEqual.unravel(), inEqual);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestGet) {
    constexpr auto in_val = 0b01101; 
    constexpr ExtIntWrapper<5, false> in{ in_val };
    constexpr auto a = in.template get<0>();
    constexpr auto b = in.template get<1>();
    constexpr auto c = in.template get<2>();
    constexpr auto d = in.template get<3>();
    constexpr auto e = in.template get<4>();

    static_assert(a.unravel(), "Error with bit 0");
    static_assert(!b.unravel(), "Error with bit 1");
    static_assert(c.unravel(), "Error with bit 2");
    static_assert(d.unravel(), "Error with bit 3");
    static_assert(!e.unravel(), "Error with bit 4");
}
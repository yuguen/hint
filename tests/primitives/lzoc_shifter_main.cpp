#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE VivadoBackendTestModule

#define WRAPPER VivadoWrapper

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/lzoc_shifter.hpp"

using namespace  std;


BOOST_AUTO_TEST_CASE(testLzocShifter)
{
    WRAPPER<32, false> k{17};
    string s = to_string(k);
    cerr << s << endl;
    auto t = lzoc_shifter<5, 4>(k, WRAPPER<1, false>{0}, WRAPPER<1, false>{0});
    s = to_string(t);
    cerr << s << endl;
    BOOST_REQUIRE_MESSAGE(false, "THe test failed !");
}

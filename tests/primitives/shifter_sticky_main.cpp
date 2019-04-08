#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE VivadoBackendTestModule

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/shifter_sticky.hpp"

using namespace  std;


#if defined(INTEL_BACKEND)
BOOST_AUTO_TEST_CASE(testShifterVivado)
{
    IntelWrapper<32, false> k{17};
    string s = to_string(k);
    cerr << s << endl;
    auto t = shifter_sticky<32, 4, false, false, IntelWrapper>(k, IntelWrapper<4, false> {5});
    s = to_string(t);
    cerr << s << endl;
    BOOST_REQUIRE_MESSAGE(false, "The test failed !");
}
#endif

#if defined(VIVADO_BACKEND)
BOOST_AUTO_TEST_CASE(testShifterIntel)
{
    VivadoWrapper<32, false> k{17};
    string s = to_string(k);
    cerr << s << endl;
    auto t = shifter_sticky<32, 4, false, false, VivadoWrapper>(k, VivadoWrapper<4, false> {5});
    s = to_string(t);
    cerr << s << endl;
    BOOST_REQUIRE_MESSAGE(false, "The test failed !");
}
#endif

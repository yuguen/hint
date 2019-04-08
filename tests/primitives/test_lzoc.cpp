#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestPrimitives

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(testfailed)
{
    BOOST_REQUIRE_MESSAGE(false, "FAILED");
}

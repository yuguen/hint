#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExtIntBackendTestModule

#include <boost/test/unit_test.hpp>

#include "hint.hpp"

using hint::ExtIntWrapper;

BOOST_AUTO_TEST_CASE(TestInstantiation) {
    ExtIntWrapper<12, false> test;
}
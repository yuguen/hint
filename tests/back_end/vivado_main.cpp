#define BOOST_TEST_DYN_LINK   
#define BOOST_TEST_MODULE VivadoBackendTestModule

#define WRAPPER VivadoWrapper

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "hint.hpp"
#include "tools/printing.hpp"
#include "primitives/lzoc_shifter.hpp"

using namespace  std;

template<size_t N, bool is_signed, template<size_t , bool > class wrappertype>
void afficher(hint_base<N, is_signed, wrappertype> const & val)
{
    cout << val.template slice<5, 2>() << endl;
}

template<template<size_t, bool> class wrapper_type>
wrapper_type<20, false> get20low(
        hint_base<32, false, wrapper_type> const & toto
)
{
    std::cerr << "Ping" << endl;
    return toto.template slice<19, 0>();
}

WRAPPER<20, false> comp(WRAPPER<32, false> in)
{
    std::cerr << "Ping" << endl;
    auto ret = get20low(in);
    return ret;
}


BOOST_AUTO_TEST_CASE(TMP)
{
    WRAPPER<32, false> k{17};
    string s = to_string(k);
    cerr << s << endl;
    auto t = lzoc_shifter<5, 4>(k, WRAPPER<1, false>{0}, WRAPPER<1, false>{0});
    s = to_string(t);
    cerr << s << endl;
    BOOST_REQUIRE_MESSAGE(false, "THe test failed !");
}

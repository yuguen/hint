#include <boost/test/unit_test.hpp>
#include "hint.hpp"
#include "primitives/table.hpp"
#include "tools/printing.hpp"
#include "helpers/bit_sequence_generator.hpp"

using hint::VivadoWrapper;
struct Squarer {
	template<unsigned int val>
	static hint::VivadoWrapper<8, false> map() {
		return {val * val};
	}

	static unsigned int runtime_map(unsigned int val) {
		return val * val;
	}
};

BOOST_AUTO_TEST_CASE(TestTableVivado) {
	constexpr unsigned int SIZE = 4;
	using T = hint::TabulatedFunction<Squarer, SIZE, SIZE << 1, VivadoWrapper>;
	for (unsigned int i = 0; i < (1<<SIZE); ++i ) {
		VivadoWrapper<SIZE, false> key{i};
		VivadoWrapper<SIZE << 1, false> res{Squarer::runtime_map(i)};
		VivadoWrapper<SIZE << 1, false> res_tab = T::read(key);

		BOOST_REQUIRE_MESSAGE((res == res_tab).template isSet<0>(), "Error for input " << i);
	}
}

BOOST_AUTO_TEST_CASE(TestOneZerosVivado) {
	constexpr unsigned int SIZE = 4;
	constexpr unsigned int SEQ_SIZE = 1 << SIZE;
	constexpr unsigned int SEQ_MASK = (1 << SEQ_SIZE) - 1;
	unsigned int curval = SEQ_MASK;

	for(unsigned int i = 0 ; i < SEQ_SIZE ; ++i) {
		VivadoWrapper<SIZE, false> key_wrap{i};
		VivadoWrapper<SEQ_SIZE, false> ref_wrapper{curval};
		curval <<= 1;
		curval &= SEQ_MASK;
		auto test_wrap = hint::one_then_zeros<SIZE, SEQ_SIZE, VivadoWrapper>(key_wrap);
		BOOST_REQUIRE_MESSAGE((test_wrap == ref_wrapper).template isSet<0>(), "Error with iteration " << i << "\nReceived: " << hint::to_string(test_wrap) << "\nExpecting " << hint::to_string(ref_wrapper));
	}
}

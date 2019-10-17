#include <boost/test/unit_test.hpp>
#include "hint.hpp"
#include "primitives/table.hpp"

struct Squarer {
	template<unsigned int val>
	static hint::IntelWrapper<8, false> map() {
		return {val * val};
	}

	static unsigned int runtime_map(unsigned int val) {
		return val * val;
	}
};

BOOST_AUTO_TEST_CASE(TestTableIntel) {
	constexpr unsigned int SIZE = 4;
	using T = hint::TabulatedFunction<Squarer, SIZE, SIZE << 1, hint::IntelWrapper>;
	for (unsigned int i = 0; i < (1<<SIZE); ++i ) {
		hint::IntelWrapper<SIZE, false> key{i};
		hint::IntelWrapper<SIZE << 1, false> res{Squarer::runtime_map(i)};
		hint::IntelWrapper<SIZE << 1, false> res_tab = T::read(key);

		BOOST_REQUIRE_MESSAGE((res == res_tab).template isSet<0>(), "Error for input " << i);
	}
}

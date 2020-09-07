#ifndef FUNCTOOLS_HPP
#define FUNCTOOLS_HPP

#include <utility>

using std::forward;

namespace hint {

	template<class F, class A0>
	auto fold(F&&, A0&& a0) {
		return forward<A0>(a0);
	}

	template<class F, class A0, class...As>
	auto fold(F&& f, A0&&a0, As&&...as) {
		return f(forward<A0>(a0), fold(f, forward<As>(as)...));
	}

}

#endif // FUNCTOOLS_HPP

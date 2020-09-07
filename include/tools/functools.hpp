#ifndef FUNCTOOLS_HPP
#define FUNCTOOLS_HPP

#include <utility>

using std::forward;

namespace hint {

	template<class R, class F, class A0>
	A0&& fold(F&&, A0&& a0) {
		return forward<A0>(a0);
	}

	template<class R, class F, class A0, class...As>
	R fold(F&& f, A0&&a0, As&&...as) {
		return f(forward<A0>(a0), fold<R>(f, forward<As>(as)...));
	}

}

#endif // FUNCTOOLS_HPP

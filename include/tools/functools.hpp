#ifndef FUNCTOOLS_HPP
#define FUNCTOOLS_HPP

#include <utility>
#include "tools/int_sequence.hpp"

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

	template<typename ResType>
	struct BinaryOr
	{
		template<typename T1, typename T2>
		static inline ResType perform(T1 const & op1, T2 const & op2)
		{
			return op1 | op2;
		}

		template<typename T1, typename T2>
		inline ResType operator()(T1 const & op1, T2 const & op2)
		{
			return op1 | op2;
		}

	};
}

#endif // FUNCTOOLS_HPP

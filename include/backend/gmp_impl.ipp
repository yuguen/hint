#ifndef GMP_IMPL_IPP
#define GMP_IMPL_IPP

//#include <iostream>
#include <type_traits>

#include <gmp.h>
#include <gmpxx.h>

#include "tools/static_math.hpp"

using namespace std;

namespace hint {
	template <unsigned int W, bool is_signed>
	class GMPWrapper;

	template<unsigned int W, bool is_signed>
	GMPWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
			GMPWrapper<W, is_signed> const & lhs,
			GMPWrapper<W, is_signed> const & rhs
		)
	{
		return {lhs.val * rhs.val};
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	GMPWrapper<shiftedSize, isShiftedSigned> operator>>(
			GMPWrapper<shiftedSize, isShiftedSigned> const & lhs,
			GMPWrapper<shifterSize, false> const & rhs
			) {
		if (rhs.val >= shiftedSize)
			return {0};
		mpz_class final = lhs.val >> rhs.val.get_ui();
		return {final};
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	GMPWrapper<shiftedSize, isShiftedSigned> operator<<(
			GMPWrapper<shiftedSize, isShiftedSigned> const & lhs,
			GMPWrapper<shifterSize, false> const & rhs
			) {
		if (rhs.val >= shiftedSize)
			return {0};
		auto val = rhs.val.get_ui();
		mpz_class res = lhs.val;
		mpz_mul_2exp(res.get_mpz_t(), res.get_mpz_t(), val);
		mpz_t mask;
		mpz_init2(mask, shiftedSize + 1);
		mpz_setbit(mask, shiftedSize);
		mpz_mod(res.get_mpz_t(), res.get_mpz_t(), mask);
		mpz_clear(mask);
		return {res};
	}

	template<unsigned int W, bool is_signed>
	GMPWrapper<W+1, is_signed> operator+(
			GMPWrapper<W, is_signed> const & lhs,
			GMPWrapper<W, is_signed> const & rhs
		)
	{
		return {lhs.val + rhs.val};
	}

	template<unsigned int W, bool is_signed>
	GMPWrapper<W+1, is_signed> operator-(
			GMPWrapper<W, is_signed> const & lhs,
			GMPWrapper<W, is_signed> const & rhs
		)
	{
		mpz_class res = lhs.val - rhs.val;
		if (res < 0) {
			mpz_t mask;
			mpz_init2(mask, W+2);
			mpz_setbit(mask, W+1);
			mpz_add(res.get_mpz_t(), res.get_mpz_t(), mask);
			mpz_clear(mask);
		}
		return {res};
	}

	template<unsigned int W, bool is_signed>
	GMPWrapper<W, false> operator|(GMPWrapper<W, is_signed> const & rhs, GMPWrapper<W, is_signed> const & lhs)
	{
		return {lhs.get_repr() | rhs.get_repr()};
	}

	template<unsigned int W, bool is_signed>
	GMPWrapper<W, false> operator&(GMPWrapper<W, is_signed> const & rhs, GMPWrapper<W, is_signed> const & lhs)
	{
		return {lhs.get_repr() & rhs.get_repr()};
	}

	template<unsigned int W, bool is_signed>
	GMPWrapper<W, false> operator^(GMPWrapper<W, is_signed> const & rhs, GMPWrapper<W, is_signed> const & lhs)
	{
		return {lhs.get_repr() ^ rhs.get_repr()};
	}

	template <unsigned int W, bool is_signed>
	class GMPWrapper
	{
		private:
			mpz_class val;

			inline mpz_class get_repr() const
			{
				mpz_class repr = val;
				if (val < 0) {
					mpz_class offset;
					mpz_setbit(offset.get_mpz_t(), W);
					repr += offset;
				}
				return repr;
			}

		public:
			GMPWrapper(mpz_class init_val):val{init_val}
			{
				auto size = mpz_sizeinbase(init_val.get_mpz_t(), 2);
				if(size > W) {
					throw "Trying to initialise a GMPWrapper with too many bits";
				}
				if(init_val < 0) {
					if (not is_signed) {
						throw "Trying to initialise a GMPWrapper with a negative value";
					}
				}
				if (is_signed and init_val > 0) {
					bool negvalue = mpz_tstbit(val.get_mpz_t(), W-1);
					if (negvalue) {
						mpz_class offset{1};
						offset <<= W;
						val -= offset;
					}
				}
			}

			GMPWrapper():val{}{}

			template<unsigned int high, unsigned int low>
			inline GMPWrapper<high - low + 1, false> slice(
					typename enable_if<high >= low and high < W>::type* = 0
			) const
			{
				mpz_class res{get_repr() >> low};
				if(high < W-1) { //clear extra bits
					constexpr unsigned int size = high - low + 1;
					mpz_t mask;
					mpz_init2(mask, size + 1);
					mpz_setbit(mask, size);
					mpz_mod(res.get_mpz_t(), res.get_mpz_t(), mask);
					mpz_clear(mask);
				}
				return {res};
			}

			template<unsigned int idx>
			GMPWrapper<1, false> get(
			   typename enable_if<idx < W>::type* = 0
			) const
			{
				return {mpz_class{
						mpz_tstbit(get_repr().get_mpz_t(), idx)
					}};
			}

			template<unsigned int idx>
			bool isSet(
			   typename enable_if<idx < W>::type* = 0
			) const
			{
				return mpz_tstbit(get_repr().get_mpz_t(), idx);
			}

			GMPWrapper<W, false> bitwise_and(GMPWrapper<W, is_signed> rhs) const
			{
				return {rhs.get_repr() & get_repr()};
			}

			GMPWrapper<W, false> bitwise_or(GMPWrapper<W, is_signed> rhs) const
			{
				return {rhs.get_repr() | get_repr()};
			}

			GMPWrapper<W, false> bitwise_xor(GMPWrapper<W, is_signed> rhs) const
			{
				return {rhs.get_repr() ^ get_repr()};
			}

			GMPWrapper<W, false> invert() const
			{
				mpz_class ret = get_repr();
				mpz_com(ret.get_mpz_t(), ret.get_mpz_t());
				mpz_class offset;
				mpz_setbit(offset.get_mpz_t(), W);
				ret += offset;
				return {ret};
			}

			GMPWrapper<1, false> operator>(GMPWrapper<W, is_signed> const & rhs) const
			{
				return {val > rhs.val};
			}

			GMPWrapper<1, false> operator<(GMPWrapper<W, is_signed> const & rhs) const
			{
				return {val < rhs.val};
			}

			GMPWrapper<1, false> operator>=(GMPWrapper<W, is_signed> const & rhs) const
			{
				return {val >= rhs.val};
			}

			GMPWrapper<1, false> operator<=(GMPWrapper<W, is_signed> const & rhs) const
			{
				return {val <= rhs.val};
			}


			uint64_t to_uint(typename enable_if<(W <= numeric_limits<uint64_t>::digits)>::type * = 0 ) const
			{
				return mpz_get_ui(get_repr().get_mpz_t());
			}

			template<unsigned int newSize>
			GMPWrapper<newSize, false> leftpad(
					typename enable_if<(newSize >= W)>::type* = 0
					) const
			{
				return {get_repr()};
			}

			template<unsigned int Wrhs, bool isSignedRhs>
			GMPWrapper<W + Wrhs, false>
			concatenate(GMPWrapper<Wrhs, isSignedRhs> const next) const
			{
				return {(get_repr() << Wrhs) | next.val};
			}

			GMPWrapper<1, false> operator==(GMPWrapper<W, is_signed> const rhs) const {
				return {val == rhs.val};
			}

			GMPWrapper<W, is_signed>& operator=(GMPWrapper const rhs)
			{
				val = rhs.val;
				return *this;
			}

			GMPWrapper<W+1, is_signed> addWithCarry(
					GMPWrapper<W, is_signed> const op2,
					GMPWrapper<1, false> const cin
				) const
			{
				return {val + op2.val + cin.val};
			}

			GMPWrapper<W+1, is_signed> subWithCarry(
					GMPWrapper<W, is_signed> const op2,
					GMPWrapper<1, false> const cin
				) const
			{
				return {val - op2.val + cin.val};
			}

			GMPWrapper<W+1, is_signed> addWithBorrow(
					GMPWrapper<W, is_signed> const op2,
					GMPWrapper<1, false> const bin
				) const
			{
				return {val + op2.val - bin.val};
			}

			GMPWrapper<W, false> modularAdd(GMPWrapper<W, is_signed> const op2) const
			{
				mpz_class res = val + op2.val;
				mpz_t mask;
				mpz_init2(mask, W + 1);
				mpz_setbit(mask, W);
				if (res < 0) {
					mpz_add(res.get_mpz_t(), res.get_mpz_t(), mask);
				}
				mpz_mod(res.get_mpz_t(), res.get_mpz_t(), mask);
				mpz_clear(mask);
				return {res};
			}

			GMPWrapper<W, false> modularSub(GMPWrapper<W, is_signed> const op2) const
			{
				mpz_class res = val - op2.val;
				mpz_t mask;
				mpz_init2(mask, W + 1);
				mpz_setbit(mask, W);
				if (res < 0) {
					mpz_add(res.get_mpz_t(), res.get_mpz_t(), mask);
				}
				mpz_mod(res.get_mpz_t(), res.get_mpz_t(), mask);
				mpz_clear(mask);
				return {res};
			}

			static GMPWrapper<W, false> generateSequence(GMPWrapper<1, false> const fillbit)
			{
				if (mpz_tstbit(fillbit.val.get_mpz_t(), 0))
				{
					mpz_class res;
					mpz_setbit(res.get_mpz_t(), W);
					res -= 1;
					return {res};
				} else {
					return {0};
				}
			}

			GMPWrapper<W, false> as_unsigned() const
			{
				mpz_class newval{val};
				mpz_abs(newval.get_mpz_t(), newval.get_mpz_t());
				return  {newval};
			}


			GMPWrapper<1, false> or_reduction() const
			{
				return {static_cast<unsigned int>(val != 0)};
			}

			GMPWrapper<1, false> and_reduction() const
			{
				mpz_class cmp;
				mpz_setbit(cmp.get_mpz_t(), W);
				cmp -= 1;
				return {static_cast<unsigned int>(cmp == val)};
			}

			GMPWrapper<W, false> backwards() const
			{
				auto shiftsize = W - mpz_sizeinbase(val.get_mpz_t(), 2);
				auto repr = val.get_str(2);
				reverse(begin(repr), end(repr));
				mpz_class new_val{repr, 2};
				new_val <<= shiftsize;
				return {new_val};
			}

			mpz_class const unravel() const
			{
				return get_repr();
			}

			static GMPWrapper<W, is_signed> mux(
					GMPWrapper<1, false> const control,
					GMPWrapper<W, is_signed> const opt1,
					GMPWrapper<W, is_signed> const opt0
				)
			{
				mpz_class res = (control.val) ? opt1.val : opt0.val;
				return {res};
			}

			friend
			GMPWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			friend
			GMPWrapper<W+1, is_signed> operator+<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			friend
			GMPWrapper<W+1, is_signed> operator-<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			friend
			GMPWrapper<W, false> operator|<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			friend
			GMPWrapper<W, false> operator&<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			friend
			GMPWrapper<W, false> operator^<W, is_signed>(
					GMPWrapper<W, is_signed> const & lhs,
					GMPWrapper<W, is_signed> const & rhs
				);

			template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
			friend GMPWrapper<ShiftedSize, shiftedSigned> operator>>(
					GMPWrapper<ShiftedSize, shiftedSigned> const & lhs,
					GMPWrapper<shifterSize, false> const & rhs
					);

			template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
			friend GMPWrapper<ShiftedSize, shiftedSigned> operator<<(
					GMPWrapper<ShiftedSize, shiftedSigned> const & lhs,
					GMPWrapper<shifterSize, false> const & rhs
					);


			template<unsigned int N, bool val>
			friend class GMPWrapper;
	};
}


#endif // GMP_IMPL_IPP

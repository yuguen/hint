#ifndef GMP_IMPL_IPP
#define GMP_IMPL_IPP

#include <iostream>
#include <type_traits>

#include <gmp.h>
#include <gmpxx.h>

#include "tools/static_math.hpp"

using namespace std;

template <unsigned int W, bool is_signed>
class GMPWrapper;

template<unsigned int W, bool is_signed>
GMPWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
		GMPWrapper<W, is_signed> const & lhs,
		GMPWrapper<W, is_signed> const & rhs
	)
{

	mpz_class op1{lhs.val};
	mpz_class op2{rhs.val};
	if(is_signed) {
		if(lhs.negValue) {
			mpz_neg(op1.get_mpz_t(), op1.get_mpz_t());
		}
		if (rhs.negValue) {
			mpz_neg(op2.get_mpz_t(), op2.get_mpz_t());
		}
	}
	mpz_class res;
	mpz_mul(res.get_mpz_t(), op1.get_mpz_t(), op2.get_mpz_t());
	return {res};
}

template<unsigned int W, bool is_signed>
GMPWrapper<W+1, is_signed> operator+(
		GMPWrapper<W, is_signed> const & lhs,
		GMPWrapper<W, is_signed> const & rhs
	)
{
	mpz_class op1{lhs.val};
	mpz_class op2{rhs.val};
	if(is_signed) {
		if(lhs.negValue) {
			mpz_neg(op1.get_mpz_t(), op1.get_mpz_t());
		}
		if (rhs.negValue) {
			mpz_neg(op2.get_mpz_t(), op2.get_mpz_t());
		}
	}
	mpz_class res;
	mpz_add(res.get_mpz_t(), op1.get_mpz_t(), op2.get_mpz_t());
	return {res};
}


template <unsigned int W, bool is_signed>
class GMPWrapper
{
	private:
		mpz_class val;
		bool negValue;

	public:
		GMPWrapper(mpz_class init_val):val{init_val},negValue{true}
		{
			auto size = mpz_sizeinbase(init_val.get_mpz_t(), 2);
			if(size > W) {

				throw "Trying to initialise a GMPWrapper with too many bits";
			}
			negValue = init_val < 0;
			if(negValue) {
				if (not is_signed) {
					throw "Trying to initialise a GMPWrapper with a negative value";
				} else {
					mpz_abs(val.get_mpz_t(), val.get_mpz_t());
					if (size < W) {
						// Perform sign extension
						unsigned int diffval = W - size;
						mpz_class signext = 1;
						signext <<= diffval;
						signext -= 1;
						signext <<= size;
						val = signext | val;
					}
				}
			}
		}

		GMPWrapper():val{},negValue{false}{}

		template<unsigned int high, unsigned int low>
		inline GMPWrapper<high - low + 1, false> slice(
		        typename enable_if<high >= low and high < W>::type* = 0
		) const
		{
			mpz_class res{val >> low};
			if(high < W-1) { //clear extra bits
				constexpr unsigned int size = high - low + 1;
				mpz_t mask;
				mpz_init2(mask, size + 1);
				mpz_setbit(mask, size);
				mpz_sub_ui(mask, mask, 1);
				mpz_and(res.get_mpz_t(), res.get_mpz_t(), mask);
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
					mpz_tstbit(val.get_mpz_t(), idx)
				}};
		}

		template<unsigned int idx>
		bool isSet(
		   typename enable_if<idx < W>::type* = 0
		) const
		{
			return mpz_tstbit(val.get_mpz_t(), idx);
		}

		GMPWrapper<W, false> bitwise_and(GMPWrapper<W, is_signed> rhs) const
		{
			return {rhs.val & val};
		}

		GMPWrapper<W, false> bitwise_or(GMPWrapper<W, is_signed> rhs) const
		{
			return {rhs.val | val};
		}

		GMPWrapper<W, false> bitwise_xor(GMPWrapper<W, is_signed> rhs) const
		{
			return {rhs.val ^ val};
		}

		GMPWrapper<W, false> invert() const
		{
			mpz_class mask = 1;
			mpz_mul_2exp(mask.get_mpz_t(), mask.get_mpz_t(), W);
			mask -= 1;
			mpz_class res = mask ^ val;
			auto testbis = mpz_sizeinbase(res.get_mpz_t(), 2);
			return {res};
		}

		GMPWrapper<1, false> operator>(GMPWrapper<W, is_signed> const & rhs) const
		{
			mpz_class sval = val;
			if(negValue)
				mpz_neg(sval.get_mpz_t(), sval.get_mpz_t());

			mpz_class srhsval = rhs.val;
			if(rhs.negValue)
				mpz_neg(srhsval.get_mpz_t(), srhsval.get_mpz_t());
			return {sval > srhsval};
		}

		GMPWrapper<1, false> operator<(GMPWrapper<W, is_signed> const & rhs) const
		{
			mpz_class sval = val;
			if(negValue)
				mpz_neg(sval.get_mpz_t(), sval.get_mpz_t());

			mpz_class srhsval = rhs.val;
			if(rhs.negValue)
				mpz_neg(srhsval.get_mpz_t(), srhsval.get_mpz_t());
			return {sval < srhsval};
		}

		GMPWrapper<1, false> operator>=(GMPWrapper<W, is_signed> const & rhs) const
		{
			mpz_class sval = val;
			if(negValue)
				mpz_neg(sval.get_mpz_t(), sval.get_mpz_t());

			mpz_class srhsval = rhs.val;
			if(rhs.negValue)
				mpz_neg(srhsval.get_mpz_t(), srhsval.get_mpz_t());
			return {sval >= srhsval};
		}

		GMPWrapper<1, false> operator<=(GMPWrapper<W, is_signed> const & rhs) const
		{
			mpz_class sval = val;
			if(negValue)
				mpz_neg(sval.get_mpz_t(), sval.get_mpz_t());

			mpz_class srhsval = rhs.val;
			if(rhs.negValue)
				mpz_neg(srhsval.get_mpz_t(), srhsval.get_mpz_t());
			return {sval <= srhsval};
		}


		uint64_t to_uint(typename enable_if<(W <= numeric_limits<uint64_t>::digits)>::type * = 0 ) const
		{
			return mpz_get_ui(val.get_mpz_t());
		}

		template<unsigned int newSize>
		GMPWrapper<newSize, false> leftpad(
		        typename enable_if<(newSize >= W)>::type* = 0
		        ) const
		{
			return {val};
		}

		template<unsigned int Wrhs, bool isSignedRhs>
		GMPWrapper<W + Wrhs, false>
		concatenate(GMPWrapper<Wrhs, isSignedRhs> const next) const
		{
			return {(val << Wrhs) | next.val};
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
			// auto& this_ap = static_cast<storage_type const>(*this);
			// auto& op_2 = static_cast<storage_type const>(op2);
			mpz_class mask = 1;
			mask = (mask << W) - 1;
			return {(val + op2.val) & mask};
		}

		GMPWrapper<W, false> modularSub(GMPWrapper<W, is_signed> const op2) const
		{
			// auto& this_ap = static_cast<storage_type const>(*this);
			// auto& op_2 = static_cast<storage_type const>(op2);
			mpz_class mask = 1;
			mask = (mask << W) - 1;
			return {(val - op2.val) & mask};
		}

		static GMPWrapper<W, false> generateSequence(GMPWrapper<1, false> const fillbit)
		{
			if (mpz_tstbit(fillbit.val.get_mpz_t(), 0))
			{
				mpz_class res = 1;
				res <<= W;
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
			mpz_class cmp = 1;
			cmp <<= W;
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
			return val;
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

		template<unsigned int N, bool val>
		friend class GMPWrapper;
};

#endif // GMP_IMPL_IPP

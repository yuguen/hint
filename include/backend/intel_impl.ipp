#ifndef INTEL_IMPL_IPP
#define INTEL_IMPL_IPP

#include <type_traits>
#include <cstdint>

#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"
#endif
using namespace std;

#include "tools/functools.hpp"
#include "tools/static_math.hpp"
#include "tools/int_sequence.hpp"

namespace hint {
	template <unsigned int W, bool is_signed>
	class IntelWrapper;

	template<unsigned int W, bool is_signed>
	IntelWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) *
				static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	IntelWrapper<shiftedSize, isShiftedSigned> operator>>(
			IntelWrapper<shiftedSize, isShiftedSigned> const & lhs,
			IntelWrapper<shifterSize, false> const & rhs
			) {
		return static_cast<typename IntelWrapper<shiftedSize, isShiftedSigned>::storage_type const &>(lhs) >>
				static_cast<typename IntelWrapper<shifterSize, false>::storage_type const &>(rhs);
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	IntelWrapper<shiftedSize, isShiftedSigned> operator<<(
			IntelWrapper<shiftedSize, isShiftedSigned> const & lhs,
			IntelWrapper<shifterSize, false> const & rhs
			) {
		return static_cast<typename IntelWrapper<shiftedSize, isShiftedSigned>::storage_type const &>(lhs) <<
				static_cast<typename IntelWrapper<shifterSize, false>::storage_type const &>(rhs);
	}

	template<unsigned int W, bool is_signed>
	IntelWrapper<W+1, is_signed> operator+(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) +
				static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
	}

	template<unsigned int W, bool is_signed>
	IntelWrapper<W+1, is_signed> operator-(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return typename IntelWrapper<W+1, is_signed>::storage_type{
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) -
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs)
		};
	}

	template<unsigned int W, bool is_signed>
	IntelWrapper<W, false> operator&(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) &
				static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
	}

	template<unsigned int W, bool is_signed>
	IntelWrapper<W, false> operator|(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) |
				static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
	}

	template<unsigned int W, bool is_signed>
	IntelWrapper<W, false> operator^(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		)
	{
		return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) ^
				static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
	}

	template <unsigned int W, bool is_signed>
	class IntelWrapper : private ac_int<W, is_signed>
	{
	public:
		typedef IntelWrapper<W, is_signed> type;
		typedef ac_int<W, is_signed> storage_type;
		template<unsigned int N>
		using storage_helper = ac_int<N, is_signed>;
		template<unsigned int N>
		using us_storage_helper = ac_int<N, false>;
		template<unsigned int N>
		using signed_storage_helper = ac_int<N, true>;
		template<unsigned int N>
		using wrapper_helper = IntelWrapper<N, is_signed>;
		template<unsigned int N>
		using us_wrapper_helper = IntelWrapper<N, false>;
		template<unsigned int N>
		using signed_wrapper_helper = IntelWrapper<N, true>;

		IntelWrapper():storage_type{0}{}


		IntelWrapper(storage_type const & val):storage_type{val}{
		}

		template<unsigned int high, unsigned int low>
		inline IntelWrapper<high - low + 1, false> slice(
			typename enable_if<high >= low and high < W>::type* = 0
		) const
		{
			return us_storage_helper<high-low+1>{
				storage_type::template slc<high - low + 1>(low)
			};
		}

		template<unsigned int idx>
		inline IntelWrapper<1, false> get(
		   typename enable_if<idx < W>::type* = 0
		) const
		{
		   return us_storage_helper<1>{
				  storage_type::template slc<1>(idx)
			};
		}

		inline IntelWrapper<1, false> operator>(type const & rhs) const
		{
			us_wrapper_helper<1> ret{storage_type::operator>(rhs)};
			return ret;
		}

		inline IntelWrapper<1, false> operator>=(type const & rhs) const
		{
			us_wrapper_helper<1> ret{storage_type::operator>=(rhs)};
			return ret;
		}

		template<typename... elem>
		inline IntelWrapper<1, false> select_or_reduce(Sequence<elem...> ) const
		{
			auto downcast = static_cast<storage_type const &>(*this);
			auto res = fold<us_storage_helper<1>>(std::logical_or<us_storage_helper<1>>{}, downcast[elem::val]...);
			return us_wrapper_helper<1>{res};
		}

		inline IntelWrapper<1, false> operator<=(type const & rhs) const
		{
			us_wrapper_helper<1> ret{storage_type::operator<=(rhs)};
			return ret;
		}

		inline IntelWrapper<1, false> operator<(type const & rhs) const
		{
			us_wrapper_helper<1> ret{storage_type::operator<(rhs)};
			return ret;
		}

		template<unsigned int idx>
		inline bool isSet(
		   typename enable_if<idx < W>::type* = 0
		) const
		{
			// auto& this_ac = static_cast<storage_type const &>(*this);
			return ((*this).template slc<1>(idx) == 1);
		}

		inline IntelWrapper<W, false> bitwise_and(IntelWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this) & rhs};
		}

		inline IntelWrapper<W, false> bitwise_or(IntelWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this) | rhs};
		}

		inline IntelWrapper<W, false> bitwise_xor(IntelWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this) ^ rhs};
		}

		inline IntelWrapper<W, false> invert() const
		{
			return us_storage_helper<W>{~(*this)};
		}

		template<unsigned int newSize>
		inline IntelWrapper<newSize, is_signed> leftpad(
				) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			static_assert ((newSize >= W), "Trying to leftpad to a width smaller than current width. See slice instead.");
			storage_helper<W> unsigned_this = (*this);
			storage_helper<newSize> ret = unsigned_this;
			return ret;
		}

		template<unsigned int Wrhs, bool isSignedRhs>
		inline IntelWrapper<W + Wrhs, false>
		concatenate(IntelWrapper<Wrhs, isSignedRhs> const val) const
		{
			/*
			ac_int<W + Wrhs, false> res{0};
			ac_int<Wrhs, isSignedRhs> val_ap{val};
			storage_type copy{0};//TODO
			res. range<Wrhs-1, 0>() = val_ap. range<Wrhs-1, 0>();
			auto slice = copy. range<W-1, 0>();
			auto res_slc = res. range<Wrhs + W - 1, Wrhs>();
			res_slc = slice;
			*/
			auto this_ap = static_cast<ac_int<W + Wrhs, false> const>(*this);
			auto val_ap = static_cast<ac_int<Wrhs, isSignedRhs> const>(val);
			ac_int<32, false> const wrhsac{Wrhs};
			return { (this_ap << wrhsac) | val_ap };

/*			return (static_cast<storage_type const &>(*this) << Wrhs) |
				static_cast<ac_int<Wrhs, isSignedRhs> const & >(val).template slc<Wrhs-1>(0);       */
		}


		inline IntelWrapper<1, false> operator==(IntelWrapper<W, is_signed> const rhs) const {
			return us_storage_helper<1>{(storage_type::operator==(rhs))};
		}

		inline IntelWrapper<W, is_signed>& operator=(IntelWrapper const rhs)
		{
			// auto& this_ap = static_cast<storage_type&>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(rhs);
			storage_type::operator=(rhs);
			return *this;
		}

		static inline IntelWrapper<W, false> generateSequence(IntelWrapper<1, false> const val)
		{
			// auto& to_fill = static_cast<storage_type const &>(val);
			us_storage_helper<W> zero{0};
			us_storage_helper<W> ret;
			if(val == 1){
				ret = us_storage_helper<W>{~zero};
			}
			else{
				ret = zero;
			}
			return ret;
		}

		inline IntelWrapper<W+1, is_signed> addWithCarry(
				IntelWrapper<W, is_signed> const op2,
				IntelWrapper<1, false> const cin
			) const
		{
			auto op2_ap = static_cast<ac_int<W+1, is_signed> const>(op2);
			auto op2_sh_filled  = (op2_ap << 1) | ac_int<1, false>{1};
			auto cin_ap = static_cast<ac_int<1, false> const &>(cin);
			auto this_ap = static_cast<ac_int<W+1, is_signed> const> (*this);
			auto this_ext = (this_ap << 1) | cin_ap;
			storage_helper<W+2> res_ext{this_ext + op2_sh_filled};
			storage_helper<W+1> res{res_ext.template slc<W+1>(1)};
			return {res};
		}

		inline IntelWrapper<W+1, is_signed> subWithCarry(
				IntelWrapper<W, is_signed> const op2,
				IntelWrapper<1, false> const cin
			) const
		{
			// auto& op1_ap = static_cast<storage_type const &>(*this);
			// auto& op2_ap = static_cast<storage_type const &>(op2);
			// auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
			auto res = storage_type::operator-(op2) + cin;
			return storage_helper<W+1>{res};
		}

		inline IntelWrapper<W+1, is_signed> addWithBorrow(
				IntelWrapper<W, is_signed> const op2,
				IntelWrapper<1, false> const bin
			) const
		{
			auto res = storage_type::operator+(op2) - bin;
			return storage_helper<W+1>{res};
		}

		inline IntelWrapper<W, false> modularAdd(IntelWrapper<W, is_signed> const op2) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& op_2 = static_cast<storage_type const &>(op2);
			return us_storage_helper<W>{(*this) + op2};
		}

		inline IntelWrapper<W, false> modularSub(IntelWrapper<W, is_signed> const op2) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& op_2 = static_cast<storage_type const &>(op2);
			return us_storage_helper<W>{(*this) - op2};
		}

		static inline IntelWrapper<W, is_signed> mux(
				IntelWrapper<1, false> const control,
				IntelWrapper<W, is_signed> const opt1,
				IntelWrapper<W, is_signed> const opt0
			)
		{
			// auto& ac_control = static_cast<us_storage_helper<1> const &>(control);
			// return (control) ? opt1 : opt0;
			ac_int<W, is_signed> result;
			if(control==1){
				result = opt1;
			}
			else{
				result = opt0;
			}
			return result;
		}

		inline us_wrapper_helper<W> as_unsigned() const
		{
			return us_storage_helper<W>{static_cast<storage_type const &>(*this)};
		}

		inline signed_wrapper_helper<W> as_signed() const
		{
			return signed_storage_helper<W>{static_cast<storage_type const &>(*this)};
		}

		inline us_wrapper_helper<1> or_reduction() const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<1>{(*this).or_reduce()};
		}

		inline us_wrapper_helper<1> nor_reduction() const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<1>{~(*this).or_reduce()};
		}

		inline us_wrapper_helper<1> and_reduction() const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<1>{(*this).and_reduce()};
		}

		inline storage_type const & unravel() const
		{
			return (*this);
		}

		inline us_wrapper_helper<W> backwards() const
		{
			// auto& this_ac = static_cast<storage_type const &>(*this);
			us_storage_helper<W> out;
			#pragma unroll W
			for(unsigned int i = 0 ; i < W ; ++i) {
				out.template set_slc(i, (*this).template slc<1>(W - i - 1));
			}
			return out;
		}

		friend
		IntelWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		IntelWrapper<W+1, is_signed> operator+<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		IntelWrapper<W+1, is_signed> operator-<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		IntelWrapper<W, false> operator|<W, is_signed>(
				IntelWrapper<W, is_signed> const & lhs,
				IntelWrapper<W, is_signed> const & rhs
			);

		friend
		IntelWrapper<W, false> operator&<W, is_signed>(
				IntelWrapper<W, is_signed> const & lhs,
				IntelWrapper<W, is_signed> const & rhs
			);

		friend
		IntelWrapper<W, false> operator^<W, is_signed>(
				IntelWrapper<W, is_signed> const & lhs,
				IntelWrapper<W, is_signed> const & rhs
			);

		template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
		friend IntelWrapper<ShiftedSize, shiftedSigned> operator>>(
				IntelWrapper<ShiftedSize, shiftedSigned> const & lhs,
				IntelWrapper<shifterSize, false> const & rhs
				);

		template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
		friend IntelWrapper<ShiftedSize, shiftedSigned> operator<<(
				IntelWrapper<ShiftedSize, shiftedSigned> const & lhs,
				IntelWrapper<shifterSize, false> const & rhs
				);

		template<unsigned int N, bool val>
		friend class IntelWrapper;
	};
}
#endif

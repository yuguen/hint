#ifndef CATAPULT_IMPL_IPP
#define CATAPULT_IMPL_IPP

#include <type_traits>
#include <cstdint>


#include "ac_int.h"
using namespace std;

#include "tools/static_math.hpp"

namespace hint {
	template <unsigned int W, bool is_signed>
	class CatapultWrapper;

	template<unsigned int W, bool is_signed>
	CatapultWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return	lhs._val * rhs._val;
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	CatapultWrapper<shiftedSize, isShiftedSigned> operator>>(
			CatapultWrapper<shiftedSize, isShiftedSigned> const & lhs,
			CatapultWrapper<shifterSize, false> const & rhs
			) {
		return lhs._val >> rhs._val;
	}

	template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
	CatapultWrapper<shiftedSize, isShiftedSigned> operator<<(
			CatapultWrapper<shiftedSize, isShiftedSigned> const & lhs,
			CatapultWrapper<shifterSize, false> const & rhs
			) {
		return lhs._val << rhs._val;
	}

	template<unsigned int W, bool is_signed>
	CatapultWrapper<W+1, is_signed> operator+(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return	lhs._val +
				rhs._val;
	}

	template<unsigned int W, bool is_signed>
	CatapultWrapper<W+1, is_signed> operator-(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return typename CatapultWrapper<W+1, is_signed>::storage_helper{
			lhs._val -
			rhs._val
		};
	}

	template<unsigned int W, bool is_signed>
	CatapultWrapper<W, false> operator&(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return	lhs._val &
				rhs._val;
	}

	template<unsigned int W, bool is_signed>
	CatapultWrapper<W, false> operator|(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return	lhs._val |
				rhs._val;
	}

	template<unsigned int W, bool is_signed>
	CatapultWrapper<W, false> operator^(
			CatapultWrapper<W, is_signed> const & lhs,
			CatapultWrapper<W, is_signed> const & rhs
		)
	{
		return	lhs._val ^
				rhs._val;
	}

	template <unsigned int W, bool is_signed>
	class CatapultWrapper
	{

	private:
			ac_int<W, is_signed> _val;
	public:
		typedef CatapultWrapper<W, is_signed> type;
		typedef ac_int<W, is_signed> storage_type;
		template<unsigned int N>
		using storage_helper = ac_int<N, is_signed>;
		template<unsigned int N>
		using us_storage_helper = ac_int<N, false>;
		template<unsigned int N>
		using signed_storage_helper = ac_int<N, true>;
		template<unsigned int N>
		using wrapper_helper = CatapultWrapper<N, is_signed>;
		template<unsigned int N>
		using us_wrapper_helper = CatapultWrapper<N, false>;
		template<unsigned int N>
		using signed_wrapper_helper = CatapultWrapper<N, true>;

		CatapultWrapper():_val{0}{}

		CatapultWrapper(ac_int<W, is_signed> init_val):_val{init_val}
		{
			// _val = init_val;
		}


		template<unsigned int high, unsigned int low>
		inline CatapultWrapper<high - low + 1, false> slice(
			typename enable_if<high >= low and high < W>::type* = 0
		) const
		{
			return CatapultWrapper<high-low+1, false>{
				_val.template slc<high - low + 1>(low)
			};
		}

		template<unsigned int idx>
		inline CatapultWrapper<1, false> get(
		   typename enable_if<idx < W>::type* = 0
		) const
		{
		   return us_storage_helper<1>{
				  _val.template slc<1>(idx)
			};
		}

		inline CatapultWrapper<1, false> operator>(type const & rhs) const
		{
			us_wrapper_helper<1> ret{_val>(rhs._val)};
			return ret;
		}

		inline CatapultWrapper<1, false> operator>=(type const & rhs) const
		{
			us_wrapper_helper<1> ret{_val>=(rhs._val)};
			return ret;
		}

		inline CatapultWrapper<1, false> operator<=(type const & rhs) const
		{
			us_wrapper_helper<1> ret{_val<=(rhs._val)};
			return ret;
		}

		inline CatapultWrapper<1, false> operator<(type const & rhs) const
		{
			us_wrapper_helper<1> ret{_val<(rhs._val)};
			return ret;
		}

		template<unsigned int idx>
		inline bool isSet(
		   typename enable_if<idx < W>::type* = 0
		) const
		{
			// auto& this_ac = static_cast<storage_type const &>(*this);
			return ((*this)._val.template slc<1>(idx) == 1);
		}

		inline CatapultWrapper<W, false> bitwise_and(CatapultWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this)._val & rhs._val};
		}

		inline CatapultWrapper<W, false> bitwise_or(CatapultWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this)._val | rhs._val};
		}

		inline CatapultWrapper<W, false> bitwise_xor(CatapultWrapper<W, is_signed> rhs) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<W>{(*this)._val ^ rhs._val};
		}

		inline CatapultWrapper<W, false> invert() const
		{
			return us_storage_helper<W>{~(*this)._val};
		}

		template<unsigned int newSize>
		inline CatapultWrapper<newSize, is_signed> leftpad(
				) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			static_assert ((newSize >= W), "Trying to leftpad to a width smaller than current width. See slice instead.");
			storage_helper<W> unsigned_this = (*this)._val;
			storage_helper<newSize> ret = unsigned_this;
			return ret;
		}

		template<unsigned int Wrhs, bool isSignedRhs>
		inline CatapultWrapper<W + Wrhs, false>
		concatenate(CatapultWrapper<Wrhs, isSignedRhs> const val) const
		{
			ac_int<W + Wrhs, false> res;
			res.set_slc(0, val._val);
			res.set_slc(Wrhs, _val);

			return res;
		}


		inline CatapultWrapper<1, false> operator==(CatapultWrapper<W, is_signed> const rhs) const {
			return us_storage_helper<1>{(_val==(rhs._val))};
		}

		inline CatapultWrapper<W, is_signed>& operator=(CatapultWrapper const rhs)
		{
			// auto& this_ap = static_cast<storage_type&>(*this);
			// auto& rhs_ap = static_cast<storage_type const &>(rhs);
			_val=(rhs._val);
			return *this;
		}

		static inline CatapultWrapper<W, false> generateSequence(CatapultWrapper<1, false> const val)
		{
			// auto& to_fill = static_cast<storage_type const &>(val);
			us_storage_helper<W> zero{0};
			us_storage_helper<W> ret;
			if(val._val == 1){
				ret = us_storage_helper<W>{~zero};
			}
			else{
				ret = zero;
			}
			return ret;
		}

		inline CatapultWrapper<W+1, is_signed> addWithCarry(
				CatapultWrapper<W, is_signed> const op2,
				CatapultWrapper<1, false> const cin
			) const
		{
			// auto& op1_ap = static_cast<storage_type const &>(*this);
			// auto& op2_ap = static_cast<storage_type const &>(op2);
			// auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
			auto res = _val+(op2._val) + cin._val;
			return storage_helper<W+1>{res};
		}

		inline CatapultWrapper<W+1, is_signed> subWithCarry(
				CatapultWrapper<W, is_signed> const op2,
				CatapultWrapper<1, false> const cin
			) const
		{
			// auto& op1_ap = static_cast<storage_type const &>(*this);
			// auto& op2_ap = static_cast<storage_type const &>(op2);
			// auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
			auto res = _val-(op2._val) + cin._val;
			return storage_helper<W+1>{res};
		}

		inline CatapultWrapper<W+1, is_signed> addWithBorrow(
				CatapultWrapper<W, is_signed> const op2,
				CatapultWrapper<1, false> const bin
			) const
		{
			auto res = _val+(op2._val) - bin._val;
			return storage_helper<W+1>{res};
		}

		inline CatapultWrapper<W, false> modularAdd(CatapultWrapper<W, is_signed> const op2) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& op_2 = static_cast<storage_type const &>(op2);
			return us_storage_helper<W>{(*this)._val + op2._val};
		}

		inline CatapultWrapper<W, false> modularSub(CatapultWrapper<W, is_signed> const op2) const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			// auto& op_2 = static_cast<storage_type const &>(op2);
			return us_storage_helper<W>{(*this)._val - op2._val};
		}

		static inline CatapultWrapper<W, is_signed> mux(
				CatapultWrapper<1, false> const control,
				CatapultWrapper<W, is_signed> const opt1,
				CatapultWrapper<W, is_signed> const opt0
			)
		{
			// auto& ac_control = static_cast<us_storage_helper<1> const &>(control);
			// return (control) ? opt1 : opt0;
			ac_int<W, is_signed> result;
			if(control._val==1){
				result = opt1._val;
			}
			else{
				result = opt0._val;
			}
			return result;
		}

		inline us_wrapper_helper<W> as_unsigned() const
		{
			return us_storage_helper<W>{(*this)._val};
		}

		inline signed_wrapper_helper<W> as_signed() const
		{
			return signed_storage_helper<W>{(*this)._val};
		}

		inline us_wrapper_helper<1> or_reduction() const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<1>{(*this)._val.or_reduce()};
		}

		inline us_wrapper_helper<1> and_reduction() const
		{
			// auto& this_ap = static_cast<storage_type const &>(*this);
			return us_storage_helper<1>{(*this)._val.and_reduce()};
		}

		inline storage_type const & unravel() const
		{
			return (*this)._val;
		}

		inline us_wrapper_helper<W> backwards() const
		{
			// auto& this_ac = static_cast<storage_type const &>(*this);
			us_storage_helper<W> out;
			#pragma unroll W
			for(unsigned int i = 0 ; i < W ; ++i) {
				out.template set_slc(i, (*this)._val.template slc<1>(W - i - 1));
			}
			return out;
		}

		friend
		CatapultWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		CatapultWrapper<W+1, is_signed> operator+<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		CatapultWrapper<W+1, is_signed> operator-<W, is_signed>(
				type const & lhs,
				type const & rhs
			);

		friend
		CatapultWrapper<W, false> operator|<W, is_signed>(
				CatapultWrapper<W, is_signed> const & lhs,
				CatapultWrapper<W, is_signed> const & rhs
			);

		friend
		CatapultWrapper<W, false> operator&<W, is_signed>(
				CatapultWrapper<W, is_signed> const & lhs,
				CatapultWrapper<W, is_signed> const & rhs
			);

		friend
		CatapultWrapper<W, false> operator^<W, is_signed>(
				CatapultWrapper<W, is_signed> const & lhs,
				CatapultWrapper<W, is_signed> const & rhs
			);

		template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
		friend CatapultWrapper<ShiftedSize, shiftedSigned> operator>>(
				CatapultWrapper<ShiftedSize, shiftedSigned> const & lhs,
				CatapultWrapper<shifterSize, false> const & rhs
				);

		template<unsigned int ShiftedSize, bool shiftedSigned, unsigned int shifterSize>
		friend CatapultWrapper<ShiftedSize, shiftedSigned> operator<<(
				CatapultWrapper<ShiftedSize, shiftedSigned> const & lhs,
				CatapultWrapper<shifterSize, false> const & rhs
				);

		template<unsigned int N, bool val>
		friend class CatapultWrapper;
	};
}
#endif

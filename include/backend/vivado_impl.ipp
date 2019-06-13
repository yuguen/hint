#ifndef VIVADO_IMPL_IPP
#define VIVADO_IMPL_IPP

#include <cstdint>
#include <limits>
#include <type_traits>

#include "ap_int.h"
#include "tools/static_math.hpp"

using namespace std;

template<unsigned int W, bool is_signed>
struct VivadoBaseType{};

template<unsigned int W>
struct VivadoBaseType<W, true>
{
    typedef ap_int<W> type;
};

template<unsigned int W>
struct VivadoBaseType<W, false>
{
    typedef ap_uint<W> type;
};

template <unsigned int W, bool is_signed>
class VivadoWrapper;

template<unsigned int W, bool is_signed>
VivadoWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
		VivadoWrapper<W, is_signed> const & lhs,
		VivadoWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename VivadoWrapper<W, is_signed>::storage_type const &>(lhs) *
			static_cast<typename VivadoWrapper<W, is_signed>::storage_type const &>(rhs);
}

template<unsigned int W, bool is_signed>
VivadoWrapper<W+1, is_signed> operator+(
		VivadoWrapper<W, is_signed> const & lhs,
		VivadoWrapper<W, is_signed> const & rhs
	)
{
	return	{static_cast<typename VivadoWrapper<W, is_signed>::storage_type const &>(lhs) +
			static_cast<typename VivadoWrapper<W, is_signed>::storage_type const &>(rhs)};
}

template <unsigned int W, bool is_signed>
class VivadoWrapper : private VivadoBaseType<W, is_signed>::type
{
public:
	typedef VivadoWrapper<W, is_signed> type;
    typedef typename VivadoBaseType<W, is_signed>::type storage_type;
    template<unsigned int N>
    using storage_helper = typename VivadoBaseType<N, is_signed>::type;
    template<unsigned int N>
    using us_storage_helper = typename VivadoBaseType<N, false>::type;
    template<unsigned int N>
    using wrapper_helper = VivadoWrapper<N, is_signed>;
    template<unsigned int N>
    using us_wrapper_helper = VivadoWrapper<N, false>;

    VivadoWrapper():storage_type{0}{}


    VivadoWrapper(storage_type const val):storage_type{val}{
    }

    template<unsigned int high, unsigned int low>
    VivadoWrapper<high - low + 1, false> slice(
        typename enable_if<high >= low and high < W>::type* = 0
    ) const
    {
        return us_storage_helper<high-low+1>{(*this).range(high, low)};
    }

    template<unsigned int idx>
    VivadoWrapper<1, false> get(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return us_storage_helper<1>{
            storage_helper<1>{
                (*this)[idx]
            }
        };
    }

    template<unsigned int idx>
    bool isSet(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        return ((*this)[idx] == 1);
    }

    VivadoWrapper<W, false> bitwise_and(VivadoWrapper<W, is_signed> rhs) const
    {
        return us_storage_helper<W>{(*this) & rhs};
    }

    VivadoWrapper<W, false> bitwise_or(VivadoWrapper<W, is_signed> rhs) const
    {
        return us_storage_helper<W>{(*this) | rhs};
    }

    VivadoWrapper<W, false> bitwise_xor(VivadoWrapper<W, is_signed> rhs) const
    {
        return us_storage_helper<W>{(*this) ^ rhs};
    }

    VivadoWrapper<W, false> invert() const
    {
        return us_storage_helper<W>{~(*this)};
    }

	VivadoWrapper<1, false> operator>(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator>(rhs)};
		return ret;
	}

	VivadoWrapper<1, false> operator<(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator<(rhs)};
		return ret;
	}

	VivadoWrapper<1, false> operator>=(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator>=(rhs)};
		return ret;
	}

	VivadoWrapper<1, false> operator<=(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator<=(rhs)};
		return ret;
	}

	uint64_t to_uint(typename enable_if<(W <= numeric_limits<uint64_t>::digits)>::type * = 0 ) const
	{
		return static_cast<uint64_t>(static_cast<us_wrapper_helper<64> >(static_cast<us_wrapper_helper<W>>(*this)));
	}

    template<unsigned int newSize>
    VivadoWrapper<newSize, false> leftpad(
            typename enable_if<(newSize >= W)>::type* = 0
            ) const
    {
        us_storage_helper<W> unsigned_this = (*this);
        us_storage_helper<newSize> ret = unsigned_this;
        return ret;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    VivadoWrapper<W + Wrhs, false>
    concatenate(VivadoWrapper<Wrhs, isSignedRhs> const val) const
    {
        us_wrapper_helper<Wrhs + W> ret {(*this).concat(val)};
        return  ret;
    }

    VivadoWrapper<1, false> operator==(VivadoWrapper<W, is_signed> const rhs) const {
        return us_storage_helper<1>{(static_cast<storage_type const>(*this) == static_cast<storage_type const>(rhs))};
    }

    VivadoWrapper<W, is_signed>& operator=(VivadoWrapper const rhs)
    {
        // auto& this_ap = static_cast<storage_type&>(*this);
        // auto& rhs_ap = static_cast<storage_type const>(rhs);
        storage_type::operator=(rhs);
        return *this;
    }

    static VivadoWrapper<W, false> generateSequence(VivadoWrapper<1, false> const val)
    {
        // ap_int<1> sign = static_cast<storage_type const>(val);
        ap_uint<W> ext;
        ap_uint<W> zero{0};

        if(val == 1){
            ext = ~zero;
        }else{
            ext = zero;
        }
        us_storage_helper<W> ret = ext;
        return ret;
    }

    VivadoWrapper<W+1, is_signed> addWithCarry(
            VivadoWrapper<W, is_signed> const op2,
            VivadoWrapper<1, false> const cin
        ) const
    {
        // auto& op1_ap = static_cast<storage_type const>(*this);
        // auto& op2_ap = static_cast<storage_type const>(op2);
        // auto& cin_ap = static_cast<us_storage_helper<1> const>(cin);
        auto res = (*this) + op2 + cin;
        return storage_helper<W+1>{res};
    }

	VivadoWrapper<W+1, is_signed> subWithCarry(
			VivadoWrapper<W, is_signed> const op2,
			VivadoWrapper<1, false> const cin
		) const
	{
		// auto& op1_ap = static_cast<storage_type const>(*this);
		// auto& op2_ap = static_cast<storage_type const>(op2);
		// auto& cin_ap = static_cast<us_storage_helper<1> const>(cin);
		auto res = (*this) - op2 + cin;
		return storage_helper<W+1>{res};
	}

	VivadoWrapper<W+1, is_signed> addWithBorrow(
			VivadoWrapper<W, is_signed> const op2,
			VivadoWrapper<1, false> const bin
		) const
	{
		auto res = (*this) + op2 - bin;
		return storage_helper<W+1>{res};
	}

    VivadoWrapper<W, false> modularAdd(VivadoWrapper<W, is_signed> const op2) const
    {
        // auto& this_ap = static_cast<storage_type const>(*this);
        // auto& op_2 = static_cast<storage_type const>(op2);
        return us_storage_helper<W>{(*this) + op2};
    }

	VivadoWrapper<W, false> modularSub(VivadoWrapper<W, is_signed> const op2) const
	{
		// auto& this_ap = static_cast<storage_type const>(*this);
		// auto& op_2 = static_cast<storage_type const>(op2);
		return us_storage_helper<W>{(*this) - op2};
	}

    static VivadoWrapper<W, is_signed> mux(
            VivadoWrapper<1, false> const control,
            VivadoWrapper<W, is_signed> const opt1,
            VivadoWrapper<W, is_signed> const opt0
        )
    {
        storage_helper<W> res;
        if(control) {
            res = opt1;
        } else {
            res = opt0;
        }
        return res;
    }

    us_wrapper_helper<W> as_unsigned() const
    {
        // auto& this_ap = static_cast<storage_type const>(*this);
        return us_storage_helper<W>{(*this)};
    }

    us_wrapper_helper<1> or_reduction() const
    {
        // auto& this_ap = static_cast<storage_type const>(*this);
        return us_storage_helper<1>{(*this).or_reduce()};
    }

    us_wrapper_helper<1> and_reduction() const
    {
        // auto& this_ap = static_cast<storage_type const>(*this);
        return us_storage_helper<1>{(*this).and_reduce()};
    }

    us_wrapper_helper<W> backwards() const
    {
        // auto& this_ap = static_cast<storage_type const>(*this);
        us_storage_helper<W> out;
        for(unsigned int i = 0 ; i < W ; ++i) {
		#pragma HLS UNROLL
            out[i] = (*this)[W - i - 1];
        }
        return out;
    }

    storage_type const unravel() const
    {
        return (*this);
    }

	friend
	VivadoWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*<W, is_signed>(
			type const & lhs,
			type const & rhs
		);
	friend
	VivadoWrapper<W+1, is_signed> operator+<W, is_signed>(
			type const & lhs,
			type const & rhs
		);

    template<unsigned int N, bool val>
    friend class VivadoWrapper;
};



#endif

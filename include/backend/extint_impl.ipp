#ifndef EXTINT_IMPL_IPP
#define EXTINT_IMPL_IPP

#include "config.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

#include "primitives/backwards.hpp"

#include "tools/functools.hpp"
#include "tools/int_sequence.hpp"
#include "tools/static_math.hpp"

using namespace std;

namespace hint {
namespace detail {
template <unsigned int W, bool is_signed> struct ExtIntBaseType {};

template <unsigned int W> struct ExtIntBaseType<W, true> {
  using type = _ExtInt(W);
};

template <unsigned int W> struct ExtIntBaseType<W, false> {
  using type = unsigned _ExtInt(W);
};

template <> struct ExtIntBaseType<1, false> { using type = bool; };

template <> struct ExtIntBaseType<1, true> { using type = bool; };
} // namespace detail

template <unsigned int W, bool is_signed> class ExtIntWrapper;

template <unsigned int W1, unsigned int W2, bool is_signed>
constexpr ExtIntWrapper<Arithmetic_Prop<W1, W2>::_prodSize, is_signed>
operator*(ExtIntWrapper<W1, is_signed> const &lhs,
          ExtIntWrapper<W2, is_signed> const &rhs) {
  return {lhs._val * rhs._val};
}

template <unsigned int shiftedSize, bool isShiftedSigned,
          unsigned int shifterSize>
constexpr ExtIntWrapper<shiftedSize, isShiftedSigned>
operator>>(ExtIntWrapper<shiftedSize, isShiftedSigned> const &lhs,
           ExtIntWrapper<shifterSize, false> const &rhs) {
  return {lhs._val >> rhs._val};
}

template <unsigned int shiftedSize, bool isShiftedSigned,
          unsigned int shifterSize>
constexpr ExtIntWrapper<shiftedSize, isShiftedSigned>
operator<<(ExtIntWrapper<shiftedSize, isShiftedSigned> const &lhs,
           ExtIntWrapper<shifterSize, false> const &rhs) {
  return {lhs._val << rhs._val};
}

template <unsigned int W, bool is_signed>
constexpr ExtIntWrapper<W + 1, is_signed>
operator+(ExtIntWrapper<W, is_signed> const &lhs,
          ExtIntWrapper<W, is_signed> const &rhs) {
  return {lhs._val + rhs._val};
}

template <unsigned int W, bool is_signed>
constexpr ExtIntWrapper<W + 1, is_signed>
operator-(ExtIntWrapper<W, is_signed> const &lhs,
          ExtIntWrapper<W, is_signed> const &rhs) {
  return {lhs._val - rhs._val};
}

#define HINT_EXTINTIMP_BINARY_OP_IMP(SYMBOL)                                   \
  template <unsigned int W, bool is_signed>                                    \
  constexpr ExtIntWrapper<W, false> operator SYMBOL(                           \
      ExtIntWrapper<W, is_signed> const &lhs,                                  \
      ExtIntWrapper<W, is_signed> const &rhs) {                                \
    return {lhs._val SYMBOL rhs._val};                                         \
  }

HINT_EXTINTIMP_BINARY_OP_IMP(|)
HINT_EXTINTIMP_BINARY_OP_IMP(&)
HINT_EXTINTIMP_BINARY_OP_IMP(^)

#undef HINT_EXTINTIMP_BINARY_OP_IMP

template <unsigned int W, bool is_signed> class ExtIntWrapper {
public:
  typedef ExtIntWrapper<W, is_signed> type;
  typedef typename detail::ExtIntBaseType<W, is_signed>::type storage_type;
  template <unsigned int N>
  using storage_helper = typename detail::ExtIntBaseType<N, is_signed>::type;
  template <unsigned int N>
  using us_storage_helper = typename detail::ExtIntBaseType<N, false>::type;
  template <unsigned int N>
  using signed_storage_helper = typename detail::ExtIntBaseType<N, true>::type;
  template <unsigned int N> using wrapper_helper = ExtIntWrapper<N, is_signed>;
  template <unsigned int N> using us_wrapper_helper = ExtIntWrapper<N, false>;
  template <unsigned int N>
  using signed_wrapper_helper = ExtIntWrapper<N, true>;
  static constexpr unsigned int Width = W;

private:
  storage_type _val;

  template <unsigned int pos>
  static constexpr us_storage_helper<W> one_at_pos() {
    return us_storage_helper<W>(1) << pos;
  }

  template <unsigned int n>
  static constexpr us_storage_helper<W> first_n_set() {
    auto successor = one_at_pos<n>();
    return successor - 1;
  }

  template <unsigned int width>
  static constexpr us_storage_helper<width>
  rec_backward(us_storage_helper<width> in) {
    if constexpr (width == 1) {
      return in;
    } else {
      constexpr auto c2pow = Static_Val<width>::_c2pow;
      constexpr auto low_width = c2pow >> 1;
      constexpr auto high_width = width - low_width;
      constexpr auto low_mask = ~us_storage_helper<low_width>{0};
      constexpr auto high_mask = (~us_storage_helper<high_width>{0})
                                 << low_width;
      us_storage_helper<high_width> high{(in & high_mask) >> low_width};
      us_storage_helper<low_width> low{in & low_mask};
      auto rlow = rec_backward(low);
      auto rhigh = rec_backward(high);
      us_storage_helper<width> new_high{rlow << high_width};
      us_storage_helper<width> res{new_high | rhigh};
      return res;
    }
  }

public:
  constexpr ExtIntWrapper() : _val{0} {}

  constexpr ExtIntWrapper(storage_type const val) : _val{val} {}

  /**
   * @brief get the bit at index idx
   *
   * @tparam idx
   * @return ExtIntWrapper<1, false>
   */
  template <unsigned int idx> constexpr ExtIntWrapper<1, false> get() const {
    static_assert(idx < W, "Checking bit outside of range");
    constexpr auto mask = one_at_pos<idx>();
    return (mask & _val);
  }

  /**
   * @brief Get a slice from the bit vector representing the stored value
   *
   * @tparam high index of the highest bit to keep in the slice
   * @tparam low index of the lowest bit to keep in the slice
   * @return ExtIntWrapper<high - low + 1, false>
   */
  template <unsigned int high, unsigned int low>
  constexpr ExtIntWrapper<high - low + 1, false> slice() const {
    static_assert(high >= low and high < W,
                  "Trying to slice outside of bounds");
    if constexpr (high == low) {
      return get<low>();
    } else {
      return static_cast<us_storage_helper<high + 1>>(_val) >> low;
    }
  }

  template <unsigned int idx> constexpr bool isSet() const {
    return get<idx>()._val;
  }

  constexpr ExtIntWrapper<W, false> invert() const {
    return us_storage_helper<W>{~_val};
  }

#define FORWARD_BITWISE_OP(OP, func_name)                                      \
  constexpr ExtIntWrapper<W, false> func_name(type const rhs) const {          \
    return {_val OP rhs._val};                                                 \
  }

  FORWARD_BITWISE_OP(&, bitwise_and)
  FORWARD_BITWISE_OP(|, bitwise_or)
  FORWARD_BITWISE_OP(^, bitwise_xor)

#undef FORWARD_BITWISE_OP

#define FORWARD_CMP_OP(OP)                                                     \
  constexpr us_wrapper_helper<1> operator OP(type const rhs) const {           \
    return {_val OP rhs._val};                                                 \
  }

  FORWARD_CMP_OP(<)
  FORWARD_CMP_OP(>)
  FORWARD_CMP_OP(<=)
  FORWARD_CMP_OP(>=)
  FORWARD_CMP_OP(==)

#undef FORWARD_CMP_OP

  constexpr us_wrapper_helper<W> as_unsigned() const {
    return {reinterpret_cast<us_storage_helper<W>>(_val)};
  }

  constexpr signed_wrapper_helper<W> as_signed() const {
    return {reinterpret_cast<signed_storage_helper<W>>(_val)};
  }

  /**
   * @brief Left pad representation with zeros
   *
   * @tparam newSize the padded output size
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> leftpad() const {
    static_assert((newSize >= W),
                  "Trying to left pad a value to a size which is smaller than "
                  "actual size. See slice instead.");
    if constexpr (newSize == W) {
      return {_val};
    } else {
      auto us = reinterpret_cast<us_storage_helper<W>>(_val);
      auto extended = static_cast<storage_helper<newSize>>(us);
      return extended;
    }
  }

  /**
   * @brief Right pad representation with zeros
   *
   * @tparam newSize the padded output size
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> rightpad() const {
    static_assert((newSize >= W),
                  "Trying to right pad a value to a size which is smaller than "
                  "actual size. See slice instead.");
    constexpr auto sizeDiff = newSize - W;
    if constexpr (sizeDiff > 0) {
      auto ret = static_cast<wrapper_helper<newSize>>(_val);
      return {ret << sizeDiff};
    } else {
      return {_val};
    }
  }

  /**
   * @brief Perform sign extension of the input (equivalent to leftpad for
   * unsigned values)
   *
   * @tparam newSize sign extended output size
   * @return VivadoWrapper<newSize, is_signed>
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> sign_extend() const {
    static_assert((newSize >= W),
                  "Trying to sign extend a value to a size which is smaller "
                  "than actual size. See slice instead.");
    return {static_cast<storage_helper<newSize>>(_val)};
  }

  /**
   * @brief Return the concatenation of the two integers
   *
   * @param val The value to append at the right of current value
   * @return us_wrapper_helper<W + Wrhs>
   */
  template <unsigned int Wrhs, bool isSignedRhs>
  constexpr us_wrapper_helper<W + Wrhs>
  concatenate(ExtIntWrapper<Wrhs, isSignedRhs> const val) const {
    constexpr auto retSize = W + Wrhs;
    auto leftmost = rightpad<retSize>();
    auto rightmost = val.template leftpad<retSize>();
    return {leftmost | rightmost};
  }

  static constexpr us_wrapper_helper<W>
  generateSequence(us_wrapper_helper<1> const val) {
    constexpr us_wrapper_helper<W> zeros{0};
    constexpr us_wrapper_helper<W> ones = zeros.invert();
    us_wrapper_helper<W> ret = (val._val) ? ones : zeros;
    return ret;
  }

  constexpr wrapper_helper<W + 1>
  addWithCarry(wrapper_helper<W> const op2,
               us_wrapper_helper<1> const cin) const {
    auto res = _val + op2._val + cin._val;
    return {res};
  }

  constexpr wrapper_helper<W + 1>
  subWithCarry(wrapper_helper<W> const op2,
               us_wrapper_helper<1> const cin) const {
    auto res = _val - op2._val + cin._val;
    return {res};
  }

  constexpr wrapper_helper<W + 1>
  addWithBorrow(wrapper_helper<W> const op2,
                us_wrapper_helper<1> const bin) const {
    auto res = _val + op2._val - bin._val;
    return {res};
  }

  constexpr us_wrapper_helper<W> modularAdd(type const op2) const {
    return {_val + op2._val};
  }

  constexpr us_wrapper_helper<W> modularSub(type const op2) const {
    return {_val - op2._val};
  }

  static constexpr type mux(us_wrapper_helper<1> const &control,
                            type const opt1, type const opt0) {
    return (control._val) ? opt1 : opt0;
  }

  constexpr us_wrapper_helper<1> or_reduction() const { return {_val != 0}; }

  constexpr us_wrapper_helper<1> nor_reduction() const { return {_val == 0}; }

  constexpr us_wrapper_helper<1> and_reduction() const {
    return {invert()._val == 0};
  }

  constexpr us_wrapper_helper<W> backwards() const {
    return {rec_backward(_val)};
  }

  template <unsigned int W2>
  constexpr wrapper_helper<Arithmetic_Prop<W, W2>::_prodSize>
  operator*(ExtIntWrapper<W2, is_signed> const &rhs) const {
    return {_val * rhs.unravel()};
  }

  constexpr storage_type const unravel() const { return _val; }

  friend constexpr ExtIntWrapper<W + 1, is_signed> operator+
      <W, is_signed>(type const &lhs, type const &rhs);

  friend constexpr ExtIntWrapper<W + 1, is_signed> operator-
      <W, is_signed>(type const &lhs, type const &rhs);

#define HINT_EXTINT_BINARY_FRIENDOP(SYM)                                       \
  friend constexpr us_wrapper_helper<W> operator SYM<W, is_signed>(            \
      type const &lhs, type const &rhs);

  HINT_EXTINT_BINARY_FRIENDOP(&)
  HINT_EXTINT_BINARY_FRIENDOP(|)
  HINT_EXTINT_BINARY_FRIENDOP(^)
#undef HINT_EXTINT_BINARY_FRIENDOP

  template <unsigned int ShiftedSize, bool shiftedSigned,
            unsigned int shifterSize>
  friend constexpr ExtIntWrapper<ShiftedSize, shiftedSigned>
  operator>>(ExtIntWrapper<ShiftedSize, shiftedSigned> const &lhs,
             ExtIntWrapper<shifterSize, false> const &rhs);

  template <unsigned int ShiftedSize, bool shiftedSigned,
            unsigned int shifterSize>
  friend constexpr ExtIntWrapper<ShiftedSize, shiftedSigned>
  operator<<(ExtIntWrapper<ShiftedSize, shiftedSigned> const &lhs,
             ExtIntWrapper<shifterSize, false> const &rhs);

  template <unsigned int N, bool val> friend class ExtIntWrapper;
};

template <> struct Config_Values<ExtIntWrapper> {
  constexpr static unsigned int shift_group_by = 2;
};

} // namespace hint
#endif
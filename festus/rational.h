// festus/rational.h
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2018-19 Google LLC
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Rational numbers.
//
// These are implemented as fractions of the form
//
//   (-1)**s * n / d * 2**p
//
// where
//
//   s is a sign bit,
//   n is a 64-bit unsigned numerator,
//   d is a 32-bit unsigned denominator, and
//   p is a 30-bit signed exponent.
//
// The smallest positive representable Rational value is therefore:
//
//   1 / (2**32 - 1) * 2**(-2**29) ~= 1.136481862409190337 * 10**-161614258
//
// The type Rational can hold exact values of all standard integral types
// (including signed and unsigned 64-bit long long) as well as IEEE floating
// point values of single precision, double precision, and 80-bit extended
// double precision (long double). Since an 80-bit long double uses a 64-bit
// fraction and 15-bit exponent, their values fit entirely into a Rational.
//
// Rationals can be used to represent small fractions exactly. They decay gently
// to inexact values which preserve high-order bits of computation results that
// are otherwise not exactly representable as Rationals.

#ifndef FESTUS_RATIONAL_H__
#define FESTUS_RATIONAL_H__

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ostream>
#include <type_traits>

namespace festus {
namespace rational_details {

template <typename T>
constexpr T ConstexprGCD(T a, T b) {
  return (a == 0 || a == b) ? b
      : (b == 0) ? a
      : ((a & 1) == 0) ? ((b & 1)
                          ? ConstexprGCD(a >> 1, b)
                          : (ConstexprGCD(a >> 1, b >> 1) << 1))
      : ((b & 1) == 0) ? ConstexprGCD(a, b >> 1)
      : (a > b)
      ? ConstexprGCD((a - b) >> 1, b)
      : ConstexprGCD((b - a) >> 1, a);
}

// Base class that only handles the representation of rationals.
class RationalBase {
 public:
  static constexpr int min_exponent = -(1 << 29);
  static constexpr int max_exponent = (1 << 29) - 1;
  static constexpr int radix = std::numeric_limits<long double>::radix;

  RationalBase() noexcept = default;

  constexpr bool signbit() const { return bits_ & 1; }

  constexpr bool inexact() const { return bits_ & 2; }

  constexpr std::int32_t exponent() const { return bits_ >> 2; }

  constexpr std::uint64_t numerator() const { return numerator_; }

  constexpr std::uint32_t denominator() const { return denominator_; }

#if 0
  constexpr std::size_t Hash() const {
    return numerator_ ^ (numerator_ >> 32)
        ^ denominator_ ^ (static_cast<std::uint64_t>(denominator_) << 16)
        ^ (bits_ | 2);
  }

  constexpr bool Equals(RationalBase other) const {
    return numerator_ == other.numerator_
        && denominator_ == other.denominator_
        && (bits_ | 2) == (other.bits_ | 2);
  }

  constexpr bool StrictlyEquals(RationalBase other) const {
    return Equals(other) && inexact() == other.inexact();
  }
#endif

  static constexpr RationalBase Zero() {
    return RationalBase(0, kDenominatorZero, 0);
  }

  static constexpr RationalBase One() { return RationalBase(1, 1, 0); }

  static constexpr RationalBase PosInfinity() {
    return RationalBase(kNumeratorInfinity, 0, 0);
  }

  static constexpr RationalBase NegInfinity() {
    return RationalBase(kNumeratorInfinity, 0, 1);
  }

  static constexpr RationalBase NaN() { return RationalBase(0, 0, kBitsNaN); }

  template <std::int32_t exponent_value, typename T>
  static constexpr T Factor(T value, std::int32_t *exponent) {
    return (value && (value & 1) == 0)
        ? ((*exponent) += exponent_value,
           Factor<exponent_value>(value >> 1, exponent))
        : value;
  }

  static constexpr RationalBase FromULL(
      bool sign, std::uint64_t numerator, std::int32_t exponent = 0) {
    return RationalBase(Factor<1>(numerator, &exponent), 1,
                        (exponent << 2) | sign);
  }

  static constexpr RationalBase FromComponents(
      bool sign, std::uint64_t numerator, std::uint32_t denominator,
      std::int32_t exponent) {
    return RationalBase(numerator, denominator, (exponent << 2) | sign);
  }

 public:
  static constexpr std::uint64_t kNumeratorInfinity = 0x5954494E49464E49ULL;
  static constexpr std::uint32_t kDenominatorZero = 0x4F52455A;
  static constexpr std::int32_t kBitsNaN = 0x4E414E51;

  template <std::int32_t exponent_value, typename T>
  static void CancelRadixPowers(T *value, std::int32_t *exponent) {
    assert(*value);
    while ((*value & 1) == 0) {
      *exponent += exponent_value;
      *value >>= 1;
    }
  }

  template <typename T>
  static RationalBase FromFloatingPoint(T value) {
    RationalBase result(0, 0, std::signbit(value));
    if (std::isfinite(value)) {
      if (value) {
        int exponent;
        const T fraction = std::frexp(std::fabs(value), &exponent);
        auto numerator = static_cast<std::uint64_t>(
            std::scalbn(fraction, std::numeric_limits<T>::digits));
        exponent -= std::numeric_limits<T>::digits;
        CancelRadixPowers<1>(&numerator, &exponent);
        result.set_numerator(numerator);
        result.set_denominator(1);
        result.set_exponent(exponent);
      } else {
        result.set_denominator(kDenominatorZero);
      }
    } else if (std::isinf(value)) {
      result.set_numerator(kNumeratorInfinity);
    } else {
      result.set_bits(kBitsNaN);
    }
    return result;
  }

  template <typename T>
  static constexpr RationalBase FromIntegral(T value) {
    return FromULL(value < 0, value < 0 ? -value : value);
  }

  constexpr RationalBase(std::uint64_t numerator,
                         std::uint32_t denominator,
                         std::int32_t bits) noexcept
      : numerator_(numerator),
        denominator_(denominator),
        bits_(bits) {}

  template <std::uint8_t bit>
  RationalBase &SetBit(bool value) {
    constexpr std::uint32_t mask = 1 << bit;
    if (value) {
      bits_ |= mask;
    } else {
      bits_ &= ~mask;
    }
    return *this;
  }

  RationalBase &set_signbit(bool value) { return SetBit<0>(value); }

  RationalBase &set_inexact(bool value) { return SetBit<1>(value); }

  RationalBase &set_exponent(std::int32_t value) {
    if (value > max_exponent) {
      Overflow();
    } else if (value < min_exponent) {
      Underflow();
    } else {
      bits_ &= 3;
      bits_ |= value << 2;
    }
    return *this;
  }

  constexpr std::int32_t bits() const { return bits_; }

  RationalBase &set_bits(std::int32_t value) {
    bits_ = value;
    return *this;
  }

  RationalBase &set_numerator(std::uint64_t value) {
    numerator_ = value;
    return *this;
  }

  RationalBase &set_denominator(std::uint32_t value) {
    denominator_ = value;
    return *this;
  }

  std::uint64_t &mutable_numerator() { return numerator_; }

  std::uint32_t &mutable_denominator() { return denominator_; }

  void SetInfinity() {
    set_numerator(kNumeratorInfinity);
    set_denominator(0);
    set_exponent(0);
  }

  void SetZero() {
    set_numerator(0);
    set_denominator(kDenominatorZero);
    set_exponent(0);
  }

  void SetNaN() {
    set_numerator(0);
    set_denominator(0);
    bits_ &= 2;
    bits_ |= kBitsNaN;
  }

  void Overflow() {
    SetInfinity();
    set_inexact(true);
  }

  void Underflow() {
    SetZero();
    set_inexact(true);
  }

  RationalBase &Standardize() {
    if (denominator_) {
      if (numerator_) {
        std::int32_t e = exponent();
        CancelRadixPowers<1>(&numerator_, &e);
        CancelRadixPowers<-1>(&denominator_, &e);
        set_exponent(e);
      } else {
        SetZero();
      }
    } else {
      if (numerator_) {
        SetInfinity();
      } else {
        SetNaN();
      }
    }
    return *this;
  }

 private:
  std::uint64_t numerator_;
  std::uint32_t denominator_;
  std::int32_t bits_;
  //           bits_: 31...2: exponent (signed 30-bit, no bias)
  //                       1: inexact
  //                       0: sign
};  // TODO: __attribute__((aligned(16)));

}  // namespace rational_details

// Public class that leaves representation details to its base class.
class Rational : public rational_details::RationalBase {
 public:
  using Base = rational_details::RationalBase;

  Rational() noexcept = default;

#if 0
  constexpr Rational(const Rational &) noexcept = default;
  constexpr Rational(Rational &&) noexcept = default;
#endif

  constexpr Rational(RationalBase base) noexcept : Base(base) {}

  constexpr Rational(bool sign, std::uint64_t numerator,
                     std::uint32_t denominator, std::int32_t exponent) noexcept
      : Base(FromComponents(sign, numerator, denominator, exponent)) {}

  constexpr Rational(std::uint64_t numerator, std::uint32_t denominator) noexcept
      : Base(FromComponents(false, numerator, denominator, 0)) {}

  template <typename T, typename = std::enable_if<std::is_integral<T>::value>>
  constexpr Rational(T value) noexcept
      : Base(FromULL(value < 0, value < 0 ? -value : value)) {}

  Rational(float value) noexcept : Base(FromFloatingPoint(value)) {}
  Rational(double value) noexcept : Base(FromFloatingPoint(value)) {}
  Rational(long double value) noexcept : Base(FromFloatingPoint(value)) {}

  static constexpr Rational min() {
    return Base(1, std::numeric_limits<std::uint32_t>::max(),
                std::numeric_limits<std::int32_t>::min() & ~3);
  }

  static constexpr Rational lowest() { return -max(); }

  static constexpr Rational max() {
    return FromComponents(false, std::numeric_limits<std::uint64_t>::max(), 1,
                          std::numeric_limits<std::int32_t>::max() >> 2);
  }

  static constexpr Rational epsilon() {
    // TODO: Test this....
    return Rational(false, 1, std::numeric_limits<std::uint32_t>::max(), -63);
  }

  static constexpr Rational infinity() {
    return Base(1, 0, 0);
  }

  static constexpr Rational quiet_NaN() {
    return Base(0, 0, 0);
  }

  static constexpr Rational signaling_NaN() {
    return Base(0, 0, 2);
  }

  constexpr int fpclassify() const {
    using Values = int[4];
    return Values{FP_NORMAL, FP_ZERO, FP_INFINITE, FP_NAN}[
        ((denominator() == 0) << 1) | (numerator() == 0)];
    #if 0
    return denominator()
        ? (numerator() ? FP_NORMAL : FP_ZERO)
        : (numerator() ? FP_INFINITE : FP_NAN);
    #endif
  }

  constexpr bool isfinite() const { return denominator(); }

  constexpr bool isnormal() const { return denominator() && numerator(); }

  constexpr bool iszero() const { return denominator() && !numerator(); }

  constexpr bool isinf() const { return !denominator() && numerator(); }

  constexpr bool isnan() const { return !denominator() && !numerator(); }

  constexpr operator bool() const { return !denominator() || numerator(); }

  explicit operator long double() const {
    long double x = numerator();
    x /= denominator();
    x = std::scalbln(x, exponent());
    return signbit() ? -x : x;
  }

  explicit operator double() const { return static_cast<long double>(*this); }

  explicit operator float() const { return static_cast<long double>(*this); }

  constexpr Rational operator-() const {
    return Base(numerator(), denominator(), bits() ^ 1);
  }

  Rational &negate() {
    set_bits(bits() ^ 1);  // bits_ ^= 1;
    return *this;
  }

  Rational &operator+=(Rational r) {
    using rational_details::ConstexprGCD;
    // copy inexact bit
    switch (fpclassify()) {
      case FP_ZERO: return operator=(r);
      case FP_INFINITE:
        if (signbit() != r.signbit()) {
          return operator=(quiet_NaN());
        } else {
          return *this;
        }
      case FP_NAN: return *this;
      default: break;
    }
    switch (r.fpclassify()) {
      case FP_ZERO: return *this;
      case FP_INFINITE:
      case FP_NAN: return operator=(r);
      default: break;
    }
    // Reduce both fractions.
    std::uint64_t a = numerator();
    std::uint64_t b = denominator();
    std::uint64_t c = r.numerator();
    std::uint64_t d = r.denominator();
    auto gcd_a_b = ConstexprGCD(a, b);
    auto gcd_c_d = ConstexprGCD(c, d);
    a /= gcd_a_b;
    b /= gcd_a_b;
    c /= gcd_c_d;
    d /= gcd_c_d;
    // Determine common exponent.
    // Determine common denominator.
    auto gcd_b_d = ConstexprGCD(b, d);
    d *= b / gcd_b_d;  // d is now the common denominator
    // TODO
    return *this;
  }

  Rational &operator-=(Rational r) {
    return operator+=(r.negate());
  }

  // Checks for equality provided both numbers are in canonical form.
  static constexpr bool canonical_equal_to(Rational q, Rational r) {
    return q.numerator() == r.numerator()
        && q.denominator() == r.denominator()
        && q.bits() == r.bits();
  }

  static constexpr Rational CanonicalExponent(
      std::uint64_t numerator, std::uint32_t denominator,
      std::int32_t exponent) {
    return (numerator & 1) == 0
        ? CanonicalExponent(numerator >> 1, denominator, exponent + 1)
        : (denominator & 1) == 0
        ? CanonicalExponent(numerator, denominator >> 1, exponent - 1)
        : Rational(false, numerator, denominator, exponent);
  }

  static constexpr Rational Canonical(Rational r, std::uint64_t gcd) {
    return CanonicalExponent(r.numerator() / gcd, r.denominator() / gcd,
                             r.exponent());
  }

  static constexpr Rational Canonical(Rational r) {
    return Canonical(r, rational_details::ConstexprGCD<std::uint64_t>(
        r.numerator(), r.denominator()));
  }

  static constexpr bool CanonicalEqualTo(Rational q, Rational r) {
    return canonical_equal_to(Canonical(q), Canonical(r));
  }

  constexpr bool equal_to(Rational r) const {
    return denominator()
        ? (numerator()
           ? (signbit() == r.signbit() &&
              CanonicalEqualTo(*this, r))
           : !r.numerator())
        : (numerator()
           ? (r.isinf() && signbit() == r.signbit())
           : false);
  }
};

inline constexpr int fpclassify(Rational r) { return r.fpclassify(); }
inline constexpr bool isfinite(Rational r) { return r.isfinite(); }
inline constexpr bool isnormal(Rational r) { return r.isnormal(); }
inline constexpr bool isinf(Rational r) { return r.isinf(); }
inline constexpr bool isnan(Rational r) { return r.isnan(); }

inline constexpr bool signbit(Rational r) { return r.signbit(); }

inline constexpr bool operator==(Rational a, Rational b) {
  return a.equal_to(b);
}

inline constexpr bool operator!=(Rational a, Rational b) {
  return !a.equal_to(b);
}

inline Rational operator*(Rational q, Rational r) {
  using rational_details::ConstexprGCD;
  const bool sign = signbit(q) ^ signbit(r);
  switch (fpclassify(q)) {
    case FP_ZERO:
    case FP_INFINITE:
    case FP_NAN:
      return sign ? -q : q;
    default: break;
  }
  switch (fpclassify(r)) {
    case FP_ZERO:
    case FP_INFINITE:
    case FP_NAN:
      return sign ? -r : r;
    default: break;
  }
  std::uint64_t n1 = q.numerator();
  std::uint64_t n2 = r.numerator();
  std::uint64_t d = q.denominator();
  d *= r.denominator();
  auto gcd1 = ConstexprGCD(n1, d);
  n1 /= gcd1;
  d /= gcd1;
  auto gcd2 = ConstexprGCD(n2, d);
  n2 /= gcd2;
  d /= gcd2;
  std::int32_t e = q.exponent() + r.exponent();
  // Cancel powers of 2.
  while ((n1 & 1) == 0) {
    n1 >>= 1;
    e += 1;
  }
  while ((n2 & 1) == 0) {
    n2 >>= 1;
    e += 1;
  }
  while ((d & 1) == 0) {
    d >>= 1;
    e -= 1;
  }
  if (n1 <= std::numeric_limits<std::uint64_t>::max() / n2 &&
      d <= std::numeric_limits<std::uint32_t>::max()) {
    return Rational::FromComponents(sign, n1 * n2, static_cast<uint32_t>(d), e);
  }
  long double value = n1;
  value *= n2;
  value /= d;
  value = std::scalbln(value, e);  // TODO: That won't work in general!
  if (sign) value -= value;
  return Rational::FromFloatingPoint(value);  // TODO: set inexact bit!
  // return Rational::Inexact(value, exponent);
}

struct Raw {
  Raw() = delete;
  constexpr explicit Raw(Rational r) : rational(r) {}
  Rational rational;
};

struct Decimal {
  Decimal() = delete;
  constexpr explicit Decimal(Rational r) : rational(r) {}
  Rational rational;
};

inline std::ostream &operator<<(std::ostream &strm, const Raw &raw) {
  const Rational &r = raw.rational;
  return strm << (r.inexact() ? "~" : "") << (r.signbit() ? "-" : "")
              << r.numerator() << "/" << r.denominator()
              << "*" << Rational::radix << "**" << r.exponent();
}

inline bool PrintSpecialValue(std::ostream &strm, const Rational &r) {
  const auto classify = r.fpclassify();
  if (classify == FP_NAN) {
    strm << "nan";
    return true;
  }
  if (r.inexact()) strm << "~";
  if (r.signbit()) strm << "-";
  if (classify == FP_INFINITE) {
    strm << "inf";
    return true;
  } else if (classify == FP_ZERO) {
    strm << "0";
    return true;
  }
  return false;
}

inline std::ostream &operator<<(std::ostream &strm, const Rational &r) {
  if (PrintSpecialValue(strm, r)) return strm;
  strm << r.numerator();
  const auto denominator = r.denominator();
  if (denominator != 1) strm << "/" << denominator;
  const auto exponent = r.exponent();
  if (exponent) strm << "*" << Rational::radix << "**" << r.exponent();
  return strm;
}

inline std::ostream &operator<<(std::ostream &strm, const Decimal &decimal) {
  const Rational &r = decimal.rational;
  if (PrintSpecialValue(strm, r)) return strm;
  const long double x = static_cast<long double>(
      r.numerator()) / r.denominator();
  const long double log10_x =
      std::log10(x) + r.exponent() * std::log10(Rational::radix);
  const int e = static_cast<int>(std::floor(log10_x));
  strm << std::pow(10, log10_x - e) << "e" << e;
  return strm;
}

#if 0
struct RationalKeyEqual {
  bool operator()(Rational q, Rational r) const {
    int qc = q.fpclassify();
    int rc = r.fpclassify();
    switch (qc) {
      case FP_ZERO: return rc == FP_ZERO && q.signbit() == r.signbit();
      case FP_NAN: return rc == FP_NAN;
      case FP_INFINITE: return rc == FP_INFINITE && q.signbit() == r.signbit();
      default:
        return false;
    }
  }
};

struct RationalHash {
  std::size_t operator()(Rational r) const {
        std::size_t hash = 0;
    switch (r.fpclassify()) {
      case FP_ZERO: return 0;
      case FP_NAN: return 1;
      case FP_INFINITE: return 2 | r.signbit();
      default:
        return 3;
    }
  }
};
#endif

#if 0
constexpr int64 kMaxInt = 1LL << 53;

template <typename T>
constexpr bool IsWithinBounds(T x) {
  return -kMaxInt <= x && x <= kMaxInt;
}

template <typename T>
constexpr bool IsExact(T x) {
  return IsWithinBounds(x) &&
      (!std::is_floating_point<T>::value || x == static_cast<int64>(x));
}

template <typename Num, typename Denom,
          typename = std::enable_if<std::is_integral<Denom>::value>>
constexpr bool IsRepresentable(Num num, Denom denom) {
  return IsExact(num) && denom != 0;
}

template <typename T = uint64>
constexpr T ConstexprGCD(T a, T b) {
  return (a == 0 || a == b) ? b
      : (b == 0) ? a
      : (~a & 1) ? ((b & 1)
                    ? ConstexprGCD(a >> 1, b)
                    : (ConstexprGCD(a >> 1, b >> 1) << 1))
      : (~b & 1) ? ConstexprGCD(a, b >> 1)
      : (a > b) ? ConstexprGCD((a - b) >> 1, b)
      : ConstexprGCD((b - a) >> 1, a);
}

template <typename T = uint64>
T GCD(T a, T b) {
  if (a == 0) return b;
  if (b == 0) return a;
  std::size_t pow = 0;
  while (((a | b) & 1) == 0) {
    a >>= 1;
    b >>= 1;
    ++pow;
  }
  while ((a & 1) == 0) a >>= 1;
  while (b != 0) {
    while ((b & 1) == 0) b >>= 1;
    if (a > b) std::swap(a, b);
    b -= a;
  }
  a <<= pow;
  return a;
}

constexpr double NormalNumerator(double num, uint64 denom) {
  return IsRepresentable(num, denom) ?
      (static_cast<int64>(num) / ConstexprGCD(
          static_cast<uint64>(std::abs(num)), denom))
      : num / denom;
}

constexpr uint64 NormalDenominator(double num, uint64 denom) {
  return IsRepresentable(num, denom) ?
      (denom / ConstexprGCD(
          static_cast<uint64>(std::abs(num)), denom))
      : 0;
}

struct PrivateConstructor {};

template <class T>
constexpr bool FloatApproxEqual(T w1, T w2, T delta = fst::kDelta) {
  return w1 <= w2 + delta && w2 <= w1 + delta;
}

#endif

#if 0
class Rational {
 public:
  constexpr Rational() noexcept : Rational(0, 1) {}

  constexpr Rational(double num, uint64 denom = 1) noexcept
      : num_(rational_details::NormalNumerator(num, denom)),
        denom_(rational_details::NormalDenominator(num, denom)) {}

  template <int64 num = 0, uint64 denom = 1,
            typename = std::enable_if<
              rational_details::IsRepresentable(num, denom)>>
  static constexpr Rational Exact() { return Rational(num, denom); }

  static constexpr Rational Approximate(double num, double denom = 1) {
    return Rational(num / denom, 0, rational_details::PrivateConstructor());
  }

  static constexpr Rational Zero() { return Rational(0); }

  static constexpr Rational One() { return Rational(1); }

  static constexpr Rational PosInfinity() { return Rational(INFINITY); }

  static constexpr Rational NegInfinity() { return Rational(-INFINITY); }

  static constexpr Rational NumberBad() { return Rational(NAN); }

  constexpr double DoubleValue() const { return denom_ ? num_ / denom_ : num_; }

  constexpr bool Member() const { return denom_ || !std::isnan(num_); }

  std::size_t Hash() const {
    return std::hash<double>()(num_) ^ (std::hash<uint64>()(denom_) << 1);
  }

  constexpr bool Equal(Rational other) const {
    return num_ == other.num_ && denom_ == other.denom_;
  }

  constexpr bool ApproxEqual(Rational other, double delta = fst::kDelta) const {
    return rational_details::FloatApproxEqual(
        DoubleValue(), other.DoubleValue(), delta);
  }

  friend std::ostream &operator<<(std::ostream &strm, Rational q) {
    if (q.denom_) {
      return strm << static_cast<int64>(q.num_) << "/" << q.denom_;
    } else {
      return strm << q.num_;
    }
  }

  Rational &operator+=(Rational other) {
    if (denom_ &&  other.denom_) {
      // TODO: LCM
      return *this;
    }
    EnsureFloatingPoint();
    num_ += other.DoubleValue();
    return *this;
  }

  Rational &operator-=(Rational other) {
    other.num_ *= -1;
    return operator+=(other);
  }

  Rational &operator*=(Rational other) {
    if (denom_ && other.denom_) {
      // TODO
      return *this;
    }
    EnsureFloatingPoint();
    num_ *= other.DoubleValue();
    return *this;
  }

  Rational &operator/=(Rational other) {
    if (denom_ && other.denom_) {
      // TODO
      return *this;
    }
    EnsureFloatingPoint();
    num_ *= other.DoubleValue();
    return *this;
  }

  Rational &Star() {
    CheckClassInvariant();
    VLOG(0) << "Star(" << num_ << " / " << denom_ << ")";
    if (denom_) {
      if (num_ == denom_) {
        SetFloatingPoint(NAN);
        return *this;
      }
      if (num_ >= 0) {
        const uint64 positive_num = static_cast<uint64>(num_);
        DCHECK_EQ(num_, static_cast<double>(positive_num));
        if (denom_ > positive_num) {
          SetValue(denom_, denom_ - positive_num);
          return *this;
        }
      } else {
        const uint64 negative_num = static_cast<uint64>(-num_);
        DCHECK_EQ(-num_, static_cast<double>(negative_num));
        const uint64 max = std::numeric_limits<uint64>::max() - negative_num;
        if (denom_ <= max) {
          SetValue(denom_, denom_ + negative_num);
          return *this;
        }
      }
    }
    EnsureFloatingPoint();
    const double one_minus_num = 1.0 - num_;
    if (one_minus_num == 0) {
      SetFloatingPoint(NAN);
    } else {
      SetFloatingPoint(1.0 / one_minus_num);
    }
    return *this;
  }

 private:
  constexpr Rational(double num, uint64 denom,
                     rational_details::PrivateConstructor)
      : num_(num), denom_(denom) {}

  void CheckClassInvariant() const {
#ifndef NDEBUG
    if (denom_) {
      CHECK(rational_details::IsExact(num_));
      // TODO: check fully reduced
    }
#endif
  }

  void SetFloatingPoint(double num) {
    num_ = num;
    denom_ = 0;
    CheckClassInvariant();
  }

  void EnsureFloatingPoint() {
    if (denom_) SetFloatingPoint(num_ / denom_);
  }

  void SetValue(double num, int64 denom) {
    if (rational_details::IsExact(num) && denom) {
      if (denom > 0) {
        num_ = num;
        denom_ = denom;
      } else {
        num_ = -num;
        denom_ = -denom;
      }
    } else {
      num_ = num / denom;
      denom_ = 0;
    }
    CheckClassInvariant();
  }

  double num_ = 0;
  uint64 denom_ = 1;
};

bool operator==(Rational q, Rational r) {
  return q.Equal(r);
}

bool operator!=(Rational q, Rational r) {
  return !(q == r);
}

constexpr Rational kRationalZero = Rational::Exact<0, 1>();
constexpr Rational kRationalOne = Rational::Exact<1, 1>();
constexpr Rational kRationalPi = Rational::Exact<7074237752028440LL,
                                                 2251799813685248ULL>();

#endif

}  // namespace festus

namespace std {

template <>
class numeric_limits<::festus::Rational> {
 public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_singaling_NaN = true;
  static constexpr float_denorm_style has_denorm = denorm_indeterminate;
  static constexpr bool has_denorm_loss = false;

  static constexpr int min_exponent = ::festus::Rational::min_exponent;
  static constexpr int max_exponent = ::festus::Rational::max_exponent;

  static constexpr ::festus::Rational min() {
    return ::festus::Rational::min();
  }
};

}  // namespace std

#endif  // FESTUS_RATIONAL_H__

// festus/real-weight.h
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
// Copyright 2015, 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Semiring of real weights under ordinary addition and multiplication.

#ifndef FESTUS_REAL_WEIGHT_H__
#define FESTUS_REAL_WEIGHT_H__

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <istream>
#include <limits>
#include <ostream>
#include <type_traits>

#include <fst/compat.h>
#include <fst/weight.h>

namespace festus {

template <typename T>
inline bool VolatileEqualTo(volatile T v1, volatile T v2) {
  return v1 == v2;
}

template <int> struct PrecisionString {};

template <> struct PrecisionString<1> {
  static string Get() { return "8"; }
};

template <> struct PrecisionString<2> {
  static string Get() { return "16"; }
};

template <> struct PrecisionString<4> {
  static string Get() { return "32"; }
};

template <> struct PrecisionString<8> {
  static string Get() { return "64"; }
};

template <> struct PrecisionString<16> {
  static string Get() { return "128"; }
};

// The semiring of real numbers under ordinary addition and multiplication is
// defined here for illustration and testing purposes. For most practical
// applications, especially those involving stochastic FSTs, the (signed) log
// semiring should be preferred.
//
// The real semiring differs from the log semiring in several crucial regards:
//
// * The real semiring is defined over all the real numbers. By contrast, the
//   log semiring is best thought of as being isomorphic to a semiring over the
//   non-negative reals.
//
// * All and only the finite floating point values are members of this semiring.
//   Floating point values that represent infinities and NaNs are excluded.
//   (The log semiring must include at least one infinity for its
//   transformation of real 0).
//
// * The real semiring is a star semiring, i.e. its Star operation is defined to
//   satisfy the Star axiom w* == 1 + w w* == 1 + w* w. It is easy to check that
//   this axiom is satisfied for w != 1 if we define Star(w) = 1/(1-w):
//
//     1 + w w* == 1           + w . 1/(1-w)
//              == (1-w)/(1-w) + w . 1/(1-w)
//              == (1-w        + w)   /(1-w)
//              == 1/(1-w)
//              == w*
//
//   The substitution 1 == (1-w)/(1-w) is well-defined because of the assumption
//   that w != 1. Because the real semiring is commutative, the second equality
//   of the axiom follows trivially.
//
// * By contrast, the log semiring is a complete star semiring whose Star
//   operation is defined (in festus/float-weight-star.h) as an infinite sum of
//   a convergent power series (under log semiring operations). In the real
//   semiring the Star operation can be viewed as the infinite sum of a
//   geometric power series regardless of convergence. For further details, see
//   the comments for the Star() operation below.
//
// * For simplicity, the behavior of semiring operations on non-member inputs is
//   not defined rigorously, in the sense that no conclusions should be drawn
//   depending on which non-finite floating point value (positive infinity,
//   negative infinity, or NaN) an operation returns. Operations that yield
//   non-member results should be thought of as partial functions.  For example,
//   Divide(1, 0) returns positive infinity under IEEE floating point semantics,
//   which is not a member of this semiring. The semiring axioms hold when all
//   input and output values are members of this semiring, i.e. are finite
//   floating point values. For example, Zero() (real 0) in this semiring is an
//   annihilator for all finite floating point values, but Times(0, inf) and
//   Times(0, -inf) are both undefined (NaN under IEEE semantics). Similarly,
//   the Star axiom w* == 1 + w w* holds for all finite w != 1, but applies to
//   w == 1 only in the sense that both the left hand side (1*) and the right
//   hand side (1 + 1 1*) are undefined.
//
// * Beyond the Star axiom, the real semiring (over the one-point
//   compactification of the reals, and with Star(1) == inf and, improperly,
//   Star(inf) == 0) has the following additional identity:
//
//     Star(Star(Star(w))) == w
//
template <class T>
class RealWeightTpl {
 public:
  typedef RealWeightTpl ReverseWeight;

  RealWeightTpl() = default;
  ~RealWeightTpl() = default;
  RealWeightTpl(RealWeightTpl &&) = default;
  RealWeightTpl(const RealWeightTpl &) = default;
  RealWeightTpl &operator=(RealWeightTpl &&) = default;
  RealWeightTpl &operator=(const RealWeightTpl &) = default;

  constexpr RealWeightTpl(T value) : value_(value) {}

  RealWeightTpl &operator=(T value) {
    value_ = value;
    return *this;
  }

  T Value() const { return value_; }

  static const RealWeightTpl NoWeight() {
    return RealWeightTpl(std::numeric_limits<T>::quiet_NaN());
  }

  static constexpr RealWeightTpl Zero() { return RealWeightTpl(0); }

  static constexpr RealWeightTpl One() { return RealWeightTpl(1); }

  friend inline RealWeightTpl Plus(RealWeightTpl lhs, RealWeightTpl rhs) {
    lhs.value_ += rhs.value_;
    return lhs;
  }

  friend inline RealWeightTpl Minus(RealWeightTpl lhs, RealWeightTpl rhs) {
    lhs.value_ -= rhs.value_;
    return lhs;
  }

  friend inline RealWeightTpl Times(RealWeightTpl lhs, RealWeightTpl rhs) {
    lhs.value_ *= rhs.value_;
    return lhs;
  }

  friend inline RealWeightTpl Divide(RealWeightTpl lhs, RealWeightTpl rhs,
                                     fst::DivideType typ = fst::DIVIDE_ANY) {
    lhs.value_ /= rhs.value_;
    return lhs;
  }

  RealWeightTpl Quantize(float delta = fst::kDelta) const {
    if (std::isfinite(value_)) {
      return RealWeightTpl(std::floor(value_ / delta + 0.5F) * delta);
    } else {
      return *this;
    }
  }

  RealWeightTpl Reverse() const { return *this; }

  bool Member() const { return std::isfinite(value_); }

  friend inline bool operator==(const RealWeightTpl w1,
                                const RealWeightTpl w2) {
    return VolatileEqualTo<T>(w1.value_, w2.value_);
  }

  friend inline bool operator!=(const RealWeightTpl w1,
                                const RealWeightTpl w2) {
    return !VolatileEqualTo<T>(w1.value_, w2.value_);
  }

  std::size_t Hash() const { return std::hash<T>()(value_); }

  std::istream &Read(std::istream &strm) {
    static_assert(std::is_pod<T>::value, "underlying type should be POD");
    return strm.read(reinterpret_cast<char *>(&value_), sizeof(value_));
  }

  std::ostream &Write(std::ostream &strm) const {
    return strm.write(reinterpret_cast<const char *>(&value_), sizeof(value_));
  }

  static const string &Type() {
    static const string type = "real" +
        (sizeof(value_) == 4 ? "" : PrecisionString<sizeof(value_)>::Get());
    return type;
  }

  static uint64 Properties() { return fst::kSemiring | fst::kCommutative; }

 private:
  T value_;
};

template <class T>
inline bool ApproxEqual(const RealWeightTpl<T> w1,
                        const RealWeightTpl<T> w2,
                        float delta = fst::kDelta) {
  return w1.Value() <= w2.Value() + delta && w2.Value() <= w1.Value() + delta;
}

template <class T>
inline std::ostream &operator<<(std::ostream &strm, const RealWeightTpl<T> &w) {
  T f = w.Value();
  switch (std::fpclassify(f)) {
    case FP_NAN: return strm << "BadNumber";
    case FP_INFINITE: return strm << (f < 0 ? "-" : "") << "Infinity";
    default: return strm << f;
  }
}

template <class T>
inline std::istream &operator>>(std::istream &strm, RealWeightTpl<T> &w) {
  string s;
  strm >> s;
  if (s == "Infinity") {
    w = RealWeightTpl<T>(std::numeric_limits<T>::infinity());
  } else if (s == "-Infinity") {
    w = RealWeightTpl<T>(-std::numeric_limits<T>::infinity());
  } else {
    char *p;
    T f = std::strtod(s.c_str(), &p);
    if (p < s.c_str() + s.size()) {
      strm.clear(std::ios::badbit);
    } else {
      w = RealWeightTpl<T>(f);
    }
  }
  return strm;
}

// Note that the definition of Star(w) (w* for short) here coincides with the
// sum of a geometric series, including the &#x1d508; sum of a divergent series
// [https://books.google.com/books?id=fa9QaUJWLz0C&q="regular in an open"].
// Contrast this with the similar Star() operation of the log semiring:
//
// * Star() is a partial function w* == 1/(1-w) that is defined for all (finite)
//   real numbers w != 1. In particular it is defined for reals w where |w| > 1,
//   where the geometric power series diverges. In this regard Star() of the
//   real semiring here differs crucially from Star() of the log semiring, which
//   is only defined for inputs on which the geometric power series converges.
//
// * For w == 1 we define w* as infinity, which fails the Member() predicate of
//   this weight class. This is motivated by viewing the real line as having
//   been extended with a single point at (unsigned) infinity, i.e. a one-point
//   compactification as opposed to the two-point compactification with signed
//   infinities used in IEEE floating point. In the latter, it would be unclear
//   what sign the result should have, since the left and right limits of
//   1/(1-w) as w approaches 1 diverge.
template <class T>
inline RealWeightTpl<T> Star(RealWeightTpl<T> w) {
  T f = w.Value();
  if (f == 1) {
    w = std::numeric_limits<T>::infinity();
  } else if (std::isinf(f)) {
    w = static_cast<T>(0);
  } else {
    w = 1.0 / (1.0 - f);
  }
  return w;
}

// Single-precision real weight
typedef RealWeightTpl<float> RealWeight;

// Double-precision real weight
typedef RealWeightTpl<double> Real64Weight;

}  // namespace festus

#endif  // FESTUS_REAL_WEIGHT_H__

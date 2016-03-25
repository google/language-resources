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

#include <cstdlib>
#include <cmath>
#include <istream>
#include <limits>
#include <ostream>

#include "festus/weight.h"

namespace festus {

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
//   satisfy the Star axiom w* == 1 + w w* == 1 + w* w. Star() is a partial
//   function Star(w) == 1/(1-w) which is defined for all (finite) real numbers
//   w != 1 (for w == 1 we define w* as infinity, which fails the Member()
//   predicate of the real semiring). It is easy to check that this definition
//   satisfies the star axiom for any w != 1:
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
//   Note that Star(w) in the real semiring coincides with the infinite sum of
//   a geometric series, including the &#x1d508; sum of a divergent series
//   [https://books.google.com/books?id=fa9QaUJWLz0C&q="regular in an open"].
//   In particular w* is defined for reals w with |w| > 1, where the geometric
//   power series diverges. By contrast, the log semiring is a complete star
//   semiring whose Star operation is defined (in festus/float-weight-star.h)
//   as an infinite sum of a convergent power series (under log semiring
//   operations).
//
// * Operations that yield non-member results are partial functions. The
//   behavior of operations on non-member arguments is undefined. The semiring
//   axioms hold when all arguments and return values are members of this
//   semiring, i.e. are finite floating point values. For example, Zero() (real
//   0) in this semiring is an annihilator for all finite floating point values,
//   but Times(0, inf) and Times(0, -inf) are both undefined (NaN under IEEE
//   semantics), i.e. non-members. Similarly, the Star axiom w* == 1 + w w*
//   holds for all finite w != 1, but applies to w == 1 only in the sense that
//   both the left hand side (1*) and the right hand side (1 + 1 1*) are
//   non-members.
//
// * Beyond the Star axiom, the real semiring (over the one-point
//   compactification of the reals, and with Star(1) == inf and, improperly,
//   Star(inf) == 0) has the following additional identity:
//
//     Star(Star(Star(w))) == w
//
template <typename R>
struct RealSemiring {
  typedef R ValueType;

  static string Name() { return "real"; }

  static constexpr uint64 Properties() {
    return fst::kCommutative | fst::kSemiring;
  }

  static constexpr R NoWeight() { return std::numeric_limits<R>::quiet_NaN(); }
  static constexpr R Zero() { return 0; }
  static constexpr R One() { return 1; }
  static constexpr R OpPlus(R lhs, R rhs) { return lhs + rhs; }
  static constexpr R OpMinus(R lhs, R rhs) { return lhs - rhs; }
  static constexpr R OpTimes(R lhs, R rhs) { return lhs * rhs; }
  static constexpr R OpDivide(R lhs, R rhs) { return lhs / rhs; }
  static constexpr R OpStar(R val) { return 1.0 / (1.0 - val); }
  static constexpr R Reverse(R val) { return val; }

  static constexpr R Quantize(R val, float delta) {
    return Member(val) ? std::floor(val / delta + 0.5F) * delta : val;
  }

  static constexpr bool Member(R val) { return std::isfinite(val); }
  static constexpr bool NotZero(R val) { return val != 0; }

  static constexpr bool EqualTo(volatile R lhs, volatile R rhs) {
    return Member(lhs) && Member(rhs) && lhs == rhs;
  }

  static constexpr bool ApproxEqualTo(R lhs, R rhs, float delta) {
    return lhs <= rhs + delta && rhs <= lhs + delta;
  }
};

template <typename T>
using RealWeightTpl = ValueWeightStatic<RealSemiring<T>>;

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

// Single-precision real weight
typedef RealWeightTpl<float> RealWeight;

// Double-precision real weight
typedef RealWeightTpl<double> Real64Weight;

}  // namespace festus

#endif  // FESTUS_REAL_WEIGHT_H__

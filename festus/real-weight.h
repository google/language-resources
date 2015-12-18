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
// Copyright 2015 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Semiring of real weights under ordinary addition and multiplication.

#ifndef FESTUS_REAL_WEIGHT_H__
#define FESTUS_REAL_WEIGHT_H__

#include <cmath>

#include <fst/compat.h>
#include <fst/float-weight.h>

namespace fst {

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
//   The substitution 1 == (1-w)/(1-w) is well-defined becaues of the assumption
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
template <class T>
class RealWeightTpl : public FloatWeightTpl<T> {
 public:
  using FloatWeightTpl<T>::Value;

  typedef RealWeightTpl ReverseWeight;

  RealWeightTpl() : FloatWeightTpl<T>() {}

  RealWeightTpl(T f) : FloatWeightTpl<T>(f) {}

  RealWeightTpl(const RealWeightTpl<T> &w) : FloatWeightTpl<T>(w) {}

  static const RealWeightTpl<T> Zero() { return RealWeightTpl<T>(0); }

  static const RealWeightTpl<T> One() { return RealWeightTpl<T>(1); }

  static const RealWeightTpl<T> NoWeight() {
    return RealWeightTpl<T>(FloatLimits<T>::NumberBad());
  }

  static const string &Type() {
    static const string type = "real" + FloatWeightTpl<T>::GetPrecisionString();
    return type;
  }

  bool Member() const { return std::isfinite(Value()); }

  RealWeightTpl<T> Quantize(float delta = kDelta) const {
    if (std::isfinite(Value())) {
      return RealWeightTpl<T>(std::floor(Value() / delta + 0.5F) * delta);
    } else {
      return *this;
    }
  }

  RealWeightTpl<T> Reverse() const { return *this; }

  static uint64 Properties() { return kSemiring | kCommutative; }
};

// Single-precision real weight
typedef RealWeightTpl<float> RealWeight;
// Double-precision real weight
typedef RealWeightTpl<double> Real64Weight;

template <class T>
inline RealWeightTpl<T> Plus(const RealWeightTpl<T> &w1,
                             const RealWeightTpl<T> &w2) {
  return RealWeightTpl<T>(w1.Value() + w2.Value());
}

inline RealWeightTpl<float> Plus(const RealWeightTpl<float> &w1,
                                 const RealWeightTpl<float> &w2) {
  return Plus<float>(w1, w2);
}

inline RealWeightTpl<double> Plus(const RealWeightTpl<double> &w1,
                                  const RealWeightTpl<double> &w2) {
  return Plus<double>(w1, w2);
}

template <class T>
inline RealWeightTpl<T> Minus(const RealWeightTpl<T> &w1,
			      const RealWeightTpl<T> &w2) {
  return RealWeightTpl<T>(w1.Value() - w2.Value());
}

inline RealWeightTpl<float> Minus(const RealWeightTpl<float> &w1,
				  const RealWeightTpl<float> &w2) {
  return Minus<float>(w1, w2);
}

inline RealWeightTpl<double> Minus(const RealWeightTpl<double> &w1,
				   const RealWeightTpl<double> &w2) {
  return Minus<double>(w1, w2);
}

template <class T>
inline RealWeightTpl<T> Times(const RealWeightTpl<T> &w1,
                              const RealWeightTpl<T> &w2) {
  return RealWeightTpl<T>(w1.Value() * w2.Value());
}

inline RealWeightTpl<float> Times(const RealWeightTpl<float> &w1,
                                  const RealWeightTpl<float> &w2) {
  return Times<float>(w1, w2);
}

inline RealWeightTpl<double> Times(const RealWeightTpl<double> &w1,
                                   const RealWeightTpl<double> &w2) {
  return Times<double>(w1, w2);
}

template <class T>
inline RealWeightTpl<T> Divide(const RealWeightTpl<T> &w1,
                               const RealWeightTpl<T> &w2,
                               DivideType typ = DIVIDE_ANY) {
  return RealWeightTpl<T>(w1.Value() / w2.Value());
}

inline RealWeightTpl<float> Divide(const RealWeightTpl<float> &w1,
                                   const RealWeightTpl<float> &w2,
                                   DivideType typ = DIVIDE_ANY) {
  return Divide<float>(w1, w2, typ);
}

inline RealWeightTpl<double> Divide(const RealWeightTpl<double> &w1,
                                    const RealWeightTpl<double> &w2,
                                    DivideType typ = DIVIDE_ANY) {
  return Divide<double>(w1, w2, typ);
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
// * For w == 1 we define w* as NaN instead of infinity, as it is unclear what
//   sign the result should have: The limit of 1/(1-w) as w approaches 1 from
//   below is positive infinity, but the limit from above is negative infinity.
template <class T>
inline RealWeightTpl<T> Star(const RealWeightTpl<T> &w) {
  T f = w.Value();
  if (f == 1) {
    return RealWeightTpl<T>::NoWeight();
  } else {
    return RealWeightTpl<T>(1.0 / (1.0 - f));
  }
}

inline RealWeightTpl<float> Star(const RealWeightTpl<float> &w) {
  return Star<float>(w);
}

inline RealWeightTpl<double> Star(const RealWeightTpl<double> &w) {
  return Star<double>(w);
}

}  // namespace fst

#endif  // FESTUS_REAL_WEIGHT_H__

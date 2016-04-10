// festus/quaternion-semiring.h
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
// Copyright 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Noncommutative quaternion algebra.
//
// A quaternion algebra is a star semiring that is notable for its lack of
// properties:
//
//  - Plus is not idempotent;
//  - Times is not commutative;
//  - Star is not an infinite sum;
//  - the semiring is not k-closed.
//
// Depending on the underlying semiring S, this four-dimensional semimodule
// construction gives rise to the Hamiltonian quaternions (over the field of
// reals), rational quaternions (over the field of rationals), Hurwitz
// quaternions, etc.

#ifndef FESTUS_QUATERNION_SEMIRING_H__
#define FESTUS_QUATERNION_SEMIRING_H__

#include <array>
#include <ostream>
#include <utility>

#include <fst/compat.h>

#include "festus/weight.h"

namespace festus {

template <class S, class Singleton = DefaultInstance<S>>
class QuaternionSemiring {
 public:
  typedef typename S::ValueType ScalarType;
  typedef std::array<ScalarType, 4> ValueType;

  static constexpr const S &s() { return Singleton::Instance(); }

  static string Name() { return "quaternion_" + s().Name(); }

  static constexpr bool Commutative() { return false; }
  static constexpr bool Idempotent() { return false; }

  static const ValueType &NoWeight() {
    static const ValueType kNoWeight = {
      s().NoWeight(), s().NoWeight(), s().NoWeight(), s().NoWeight()
    };
    return kNoWeight;
  }

  // Initializes a quaternion q = a + bi + cj + dk from its scalar part a.
  template <class A>
  static ValueType From(A &&a) {
    ScalarType scalar(std::forward<A>(a));
    return {scalar, s().Zero(), s().Zero(), s().Zero()};
  }

  // Initializes a quaternion q = a + bi + cj + dk.
  template <class A, class B, class C, class D>
  static ValueType From(A &&a, B &&b, C &&c, D &&d) {
    ScalarType q0(std::forward<A>(a));
    ScalarType q1(std::forward<B>(b));
    ScalarType q2(std::forward<C>(c));
    ScalarType q3(std::forward<D>(d));
    return {q0, q1, q2, q3};
  }

  static const ValueType &Zero() {
    static const ValueType kZero = {s().Zero(),
                                    s().Zero(), s().Zero(), s().Zero()};
    return kZero;
  }

  static const ValueType &One() {
    static const ValueType kOne = {s().One(),
                                   s().Zero(), s().Zero(), s().Zero()};
    return kOne;
  }

  static ValueType Quantize(ValueType q, float delta) {
    q[0] = s().Quantize(q[0], delta);
    q[1] = s().Quantize(q[1], delta);
    q[2] = s().Quantize(q[2], delta);
    q[3] = s().Quantize(q[3], delta);
    return q;
  }

  static ValueType OpPlus(ValueType q, const ValueType &r) {
    q[0] = s().OpPlus(q[0], r[0]);
    q[1] = s().OpPlus(q[1], r[1]);
    q[2] = s().OpPlus(q[2], r[2]);
    q[3] = s().OpPlus(q[3], r[3]);
    return q;
  }

  static ValueType OpMinus(const ValueType &q, ValueType r) {
    // OpMinus takes its second argument by value, which is more efficient than
    // taking the first argument by value in case of e.g. OpMinus(Zero(), x) or
    // OpMinus(One(), x).
    r[0] = s().OpMinus(q[0], r[0]);
    r[1] = s().OpMinus(q[1], r[1]);
    r[2] = s().OpMinus(q[2], r[2]);
    r[3] = s().OpMinus(q[3], r[3]);
    return r;
  }

  // Conjugate quaternion. With the quaternion viewed as a scalar plus 3D
  // vector, this leaves the scalar part unchanged and replaces the vector part
  // with its opposite. When viewed as a real 4x4 (resp. complex 2x2) matrix,
  // this is the matrix (conjugate) transpose.
  static ValueType Conjugate(ValueType q) {
    q[1] = s().OpMinus(s().Zero(), q[1]);
    q[2] = s().OpMinus(s().Zero(), q[2]);
    q[3] = s().OpMinus(s().Zero(), q[3]);
    return q;
  }

  // Hamilton product of quaternions.
  static ValueType OpTimes(const ValueType &q, const ValueType &r) {
    auto a1 = q[0];
    auto b1 = q[1];
    auto c1 = q[2];
    auto d1 = q[3];

    auto a2 = r[0];
    auto b2 = r[1];
    auto c2 = r[2];
    auto d2 = r[3];

    return {
      s().OpMinus(s().OpMinus(s().OpTimes(a1, a2), s().OpTimes(b1, b2)),
                  s().OpPlus(s().OpTimes(c1, c2), s().OpTimes(d1, d2))),
      s().OpPlus(s().OpPlus(s().OpTimes(a1, b2), s().OpTimes(b1, a2)),
                 s().OpMinus(s().OpTimes(c1, d2), s().OpTimes(d1, c2))),
      s().OpPlus(s().OpMinus(s().OpTimes(a1, c2), s().OpTimes(b1, d2)),
                 s().OpPlus(s().OpTimes(c1, a2), s().OpTimes(d1, b2))),
      s().OpPlus(s().OpPlus(s().OpTimes(a1, d2), s().OpTimes(b1, c2)),
                 s().OpMinus(s().OpTimes(d1, a2), s().OpTimes(c1, b2))),
    };
  }

  // Scalar multiplication on the left.
  static ValueType ScalarTimes(ScalarType scalar, ValueType q) {
    q[0] = s().OpTimes(scalar, q[0]);
    q[1] = s().OpTimes(scalar, q[1]);
    q[2] = s().OpTimes(scalar, q[2]);
    q[3] = s().OpTimes(scalar, q[3]);
    return q;
  }

  // Quaternion squared norm.
  static constexpr ScalarType Norm2(const ValueType &q) {
    return s().OpPlus(
        s().OpPlus(s().OpTimes(q[0], q[0]), s().OpTimes(q[1], q[1])),
        s().OpPlus(s().OpTimes(q[2], q[2]), s().OpTimes(q[3], q[3])));
  }

  // Reciprocal quaternion.
  static constexpr ValueType Reciprocal(ValueType q) {
    return ScalarTimes(s().Reciprocal(Norm2(q)), Conjugate(q));
  }

  // Undirected division is undefined.
  static constexpr ValueType OpDivide(const ValueType &, const ValueType &) {
    return NoWeight();
  }

  // Star operation: Star(q) == (1 - q)^{-1} (as in any division ring).
  static constexpr ValueType OpStar(ValueType q) {
    return Reciprocal(OpMinus(One(), q));
  }

  // TODO: Figure out if this makes sense.
  static constexpr ValueType Reverse(ValueType q) { return Conjugate(q); }

  static constexpr bool Member(const ValueType &q) {
    return s().Member(q[0])
        && s().Member(q[1])
        && s().Member(q[2])
        && s().Member(q[3]);
  }

  static constexpr bool NotZero(const ValueType &q) {
    return !EqualTo(q, Zero());
  }

  static constexpr bool EqualTo(const ValueType &q, const ValueType &r) {
    return s().EqualTo(q[0], r[0])
        && s().EqualTo(q[1], r[1])
        && s().EqualTo(q[2], r[2])
        && s().EqualTo(q[3], r[3]);
  }

  static constexpr bool ApproxEqualTo(const ValueType &q, const ValueType &r,
                                      float delta) {
    return s().ApproxEqualTo(q[0], r[0], delta)
        && s().ApproxEqualTo(q[1], r[1], delta)
        && s().ApproxEqualTo(q[2], r[2], delta)
        && s().ApproxEqualTo(q[3], r[3], delta);
  }

  static std::ostream &Print(std::ostream &strm, const ValueType &q) {
    static const char *const kUnits[] = {"", "i", "j", "k"};
    bool empty = true;
    for (int i = 0; i < 4; ++i) {
      if (q[i] == 0) continue;
      bool negative = q[i] < 0;
      if (empty) {
        strm << (negative ? "-" : "");
      } else {
        strm << (negative ? " - " : " + ");
      }
      auto abs = negative ? -q[i] : q[i];
      if (i == 0) {
        strm << abs;
      } else {
        if (abs != 1) {
          strm << abs << " ";
        }
        strm << kUnits[i];
      }
      empty = false;
    }
    if (empty) strm << "0";
    return strm;
  }
};

template <class S>
using QuaternionWeightTpl = ValueWeightStatic<QuaternionSemiring<S>>;

}  // namespace festus

#endif // FESTUS_QUATERNION_SEMIRING_H__

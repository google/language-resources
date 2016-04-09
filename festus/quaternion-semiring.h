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

namespace festus {

template <class S>
class QuaternionSemiring {
 public:
  typedef std::array<typename S::ValueType, 4> ValueType;

  static string Name() { return "quaternion_" + S::Name(); }

  static constexpr bool Commutative() { return false; }
  static constexpr bool Idempotent() { return false; }

  QuaternionSemiring() = default;

  constexpr explicit QuaternionSemiring(const S *semiring) : s_(semiring) {}

  const ValueType &NoWeight() const {
    static const ValueType kNoWeight = {
      s_->NoWeight(), s_->NoWeight(), s_->NoWeight(), s_->NoWeight()
    };
    return kNoWeight;
  }

  // Initializes a quaternion q = a + bi + cj + dk from its scalar part a.
  template <class A>
  ValueType From(A &&a) const {
    typename S::ValueType scalar(std::forward<A>(a));
    ValueType result = {scalar,
                        s_->Zero(), s_->Zero(), s_->Zero()};
    return result;
  }

  // Initializes a quaternion q = a + bi + cj + dk.
  template <class A, class B, class C, class D>
  ValueType From(A &&a, B &&b, C &&c, D &&d) const {
    typename S::ValueType q0(std::forward<A>(a));
    typename S::ValueType q1(std::forward<B>(b));
    typename S::ValueType q2(std::forward<C>(c));
    typename S::ValueType q3(std::forward<C>(d));
    ValueType result = {q0, q1, q2, q3};
    return result;
  }

  const ValueType &Zero() const {
    static const ValueType kZero = {s_->Zero(),
                                    s_->Zero(), s_->Zero(), s_->Zero()};
    return kZero;
  }

  const ValueType &One() const {
    static const ValueType kOne = {s_->One(),
                                   s_->Zero(), s_->Zero(), s_->Zero()};
    return kOne;
  }

  ValueType Quantize(ValueType q, float delta) const {
    q[0] = s_->Quantize(q[0], delta);
    q[1] = s_->Quantize(q[1], delta);
    q[2] = s_->Quantize(q[2], delta);
    q[3] = s_->Quantize(q[3], delta);
    return q;
  }

  ValueType OpPlus(ValueType q, const ValueType &r) const {
    q[0] = s_->OpPlus(q[0], r[0]);
    q[1] = s_->OpPlus(q[1], r[1]);
    q[2] = s_->OpPlus(q[2], r[2]);
    q[3] = s_->OpPlus(q[3], r[3]);
    return q;
  }

  ValueType OpMinus(const ValueType &q, ValueType r) const {
    // OpMinus takes its second argument by value, which is more efficient than
    // taking the first argument by value in case of e.g. OpMinus(Zero(), x) or
    // OpMinus(One(), x).
    r[0] = s_->OpMinus(q[0], r[0]);
    r[1] = s_->OpMinus(q[1], r[1]);
    r[2] = s_->OpMinus(q[2], r[2]);
    r[3] = s_->OpMinus(q[3], r[3]);
    return r;
  }

  // Conjugate quaternion. With the quaternion viewed as a scalar plus 3D
  // vector, this leaves the scalar part unchanged and replaces the vector part
  // with its opposite. When viewed as a real 4x4 (resp. complex 2x2) matrix,
  // this is the matrix (conjugate) transpose.
  ValueType Conjugate(ValueType q) const {
    q[1] = s_->OpMinus(s_->Zero(), q[1]);
    q[2] = s_->OpMinus(s_->Zero(), q[2]);
    q[3] = s_->OpMinus(s_->Zero(), q[3]);
    return q;
  }

  // Hamilton product of quaternions.
  ValueType OpTimes(const ValueType &q, const ValueType &r) const {
    auto a1 = q[0];
    auto b1 = q[1];
    auto c1 = q[2];
    auto d1 = q[3];

    auto a2 = r[0];
    auto b2 = r[1];
    auto c2 = r[2];
    auto d2 = r[3];

    ValueType result = {
      s_->OpMinus(s_->OpMinus(s_->OpTimes(a1, a2), s_->OpTimes(b1, b2)),
                  s_->OpPlus(s_->OpTimes(c1, c2), s_->OpTimes(d1, d2))),
      s_->OpPlus(s_->OpPlus(s_->OpTimes(a1, b2), s_->OpTimes(b1, a2)),
                 s_->OpMinus(s_->OpTimes(c1, d2), s_->OpTimes(d1, c2))),
      s_->OpPlus(s_->OpMinus(s_->OpTimes(a1, c2), s_->OpTimes(b1, d2)),
                 s_->OpPlus(s_->OpTimes(c1, a2), s_->OpTimes(d1, b2))),
      s_->OpPlus(s_->OpPlus(s_->OpTimes(a1, d2), s_->OpTimes(b1, c2)),
                 s_->OpMinus(s_->OpTimes(d1, a2), s_->OpTimes(c1, b2))),
    };
    return result;
  }

  // Scalar multiplication on the left.
  ValueType ScalarTimes(typename S::ValueType scalar, ValueType q) const {
    q[0] = s_->OpTimes(scalar, q[0]);
    q[1] = s_->OpTimes(scalar, q[1]);
    q[2] = s_->OpTimes(scalar, q[2]);
    q[3] = s_->OpTimes(scalar, q[3]);
    return q;
  }

  // Quaternion squared norm.
  constexpr typename S::ValueType Norm2(const ValueType &q) {
    return s_->OpPlus(
        s_->OpPlus(s_->OpTimes(q[0], q[0]), s_->OpTimes(q[1], q[1])),
        s_->OpPlus(s_->OpTimes(q[2], q[2]), s_->OpTimes(q[3], q[3])));
  }

  // Reciprocal quaternion.
  constexpr ValueType Reciprocal(ValueType q) {
    return ScalarTimes(s_->Reciprocal(Norm2(q)), Conjugate(q));
  }

  // Undirected division is undefined.
  constexpr ValueType OpDivide(const ValueType &, const ValueType &) {
    return NoWeight();
  }

  // Star operation: Star(q) == (1 - q)^{-1} (as in any division ring).
  ValueType OpStar(ValueType q) const { return Reciprocal(OpMinus(One(), q)); }

  // TODO: Figure out if this makes sense.
  constexpr ValueType Reverse(ValueType q) { return Conjugate(q); }

  constexpr bool Member(ValueType q) {
    return s_->Member(q[0])
        && s_->Member(q[1])
        && s_->Member(q[2])
        && s_->Member(q[3]);
  }

  constexpr bool NotZero(const ValueType &q) { return !EqualTo(q, Zero()); }

  constexpr bool EqualTo(const ValueType &q, const ValueType &r) {
    return s_->EqualTo(q[0], r[0])
        && s_->EqualTo(q[1], r[1])
        && s_->EqualTo(q[2], r[2])
        && s_->EqualTo(q[3], r[3]);
  }

  constexpr bool ApproxEqualTo(
      const ValueType &q, const ValueType &r, float delta) {
    return s_->ApproxEqualTo(q[0], r[0], delta)
        && s_->ApproxEqualTo(q[1], r[1], delta)
        && s_->ApproxEqualTo(q[2], r[2], delta)
        && s_->ApproxEqualTo(q[3], r[3], delta);
  }

  std::ostream &Print(std::ostream &strm, const ValueType &q) const {
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

 private:
  const S *const s_;
};

}  // namespace festus

#endif // FESTUS_QUATERNION_SEMIRING_H__

// festus/modular-int-semiring.h
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
// The (semi)ring of integers modulo N.
//
// When N is prime, this ring is in fact a finite field. In fields (and more
// generally in division rings), Star(w) == 1/(1 - w) for all w != 1, since
// the reciprocal 1/u is defined for all u != 0. While Star() is well-defined,
// it is not the sum of a convergent series, and the semiring is not k-closed.

#ifndef FESTUS_MODULAR_INT_SEMIRING_H__
#define FESTUS_MODULAR_INT_SEMIRING_H__

#include <cinttypes>
#include <limits>
#include <ostream>
#include <sstream>

#include <fst/compat.h>

namespace festus {

// The ring (resp. field) of integers modulo N for N >= 2 (resp. N prime).
//
// The template parameter Z must be a signed integer type. Its smallest value
// is reserved for representing undefined results.
template <typename Z, int64 N>
struct ModularIntegerSemiring {
  static_assert(std::numeric_limits<Z>::is_integer, "integral type expected");
  static_assert(std::numeric_limits<Z>::is_signed, "signed type expected");
  static_assert(N >= 2, "expected N >= 2");
  static constexpr Z kMax = std::numeric_limits<Z>::max();
  static_assert(N - 1 <= kMax, "N too large");

  typedef Z ValueType;

  static string Name() {
    std::ostringstream strm;
    strm << "Z/" << N << "Z";
    return strm.str();
  }

  static constexpr bool Commutative() { return true; }
  static constexpr bool Idempotent() { return false; }

  static constexpr Z NoWeight() { return std::numeric_limits<Z>::min(); }
  static constexpr Z Zero() { return 0; }
  static constexpr Z One() { return 1; }
  static constexpr Z Reverse(Z a) { return a; }
  static constexpr Z Quantize(Z a, float) { return a; }

  // Computes a + b (mod N) without overflow.
  // TODO(C++14): Make this a constexpr function.
  static Z OpPlus(Z a, Z b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    a = Remainder(a);
    b = Remainder(b);
    if (a >= N - b) {      // a + b >= N
      return a - (N - b);
    } else {               // a + b < N
      return a + b;
    }
  }

  // Computes a - b (mod N) without overflow.
  // TODO(C++14): Make this a constexpr function.
  static Z OpMinus(Z a, Z b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    // For a remainder r, we have 0 <= r <= N-1 <= kMax. Therefore the
    // difference of two remainders is in [-kMax;+kMax], so can be safely
    // stored as type Z without overflow.
    a = Remainder(a) - Remainder(b);
    a += N * (a < 0);
    return a;
  }

  // Computes a * b (mod N) without overflow.
  static Z OpTimes(Z a, Z b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    b = Remainder(b);
    if (b == 0) return 0;
    a = Remainder(a);
    if (a <= kMax / b) {
      return Remainder(a * b);
    }
    // Russian peasant multiplication (mod N).
    Z result = 0;
    if (a > b) {
      result = a;
      a = b;
      b = result;
      result = 0;
    }
    while (a > 0) {
      if (a & 1) {
        result = OpPlus(result, b);
      }
      a >>= 1;           // a := a / 2
      b = OpPlus(b, b);  // b := b * 2 (mod N)
    }
    return result;
  }

  static Z OpDivide(Z a, Z b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    return OpTimes(a, Reciprocal(b));
  }

  static Z OpStar(Z a) {
    if (!Member(a)) return NoWeight();
    return Reciprocal(OpMinus(1, a));
  }

  static constexpr bool Member(Z a) { return a != NoWeight(); }

  static constexpr bool NotZero(Z a) {
    return !(Member(a) && (a % N) == 0);
  }

  static constexpr bool EqualTo(Z a, Z b) {
    return Member(a) && Member(b) && Remainder(a) == Remainder(b);
  }

  static constexpr bool ApproxEqualTo(Z a, Z b, float delta) {
    return EqualTo(a, b);
  }

  static std::ostream &Print(std::ostream &strm, Z a) {
    return strm << static_cast<int64>(a);
  }

  // Constructs an element of the semiring from the given integer a.
  static Z FromInteger(std::intmax_t a) {
    auto quot_rem = std::imaxdiv(a, N);
    auto r = quot_rem.rem;
    r += N * (r < 0);
    CHECK_GE(r, 0);
    CHECK_LE(r, kMax);
    CHECK_LT(r, N);
    return r;
  }

  // Computes the nonnegative remainder of a / N.
  static constexpr Z Remainder(Z a) {
    return (a %= N, a + N * (a < 0));
  }

  // Computes the multiplicative inverse via the extended Euclidean algorithm
  // in modular arithmetic.
  static Z Reciprocal(Z a) {
    if (!Member(a)) return NoWeight();
    a = Remainder(a);
    if (a == 0) return NoWeight();
    if (a == 1) return 1;
    CHECK_GE(a, 2);
    CHECK_LT(a, N);
    Z q = N / a;                  // Guaranteed to fit, even if N == kMax + 1.
    CHECK_GE(q, 1);
    Z r_prev = a, r = N - q * a;  // Loop unrolled once to make r fit into Z.
    Z s_prev = 1, s = -q;
    while (r != 0) {
      q = r_prev / r;
      CHECK_GE(q, 1);
      Z r_next = r_prev - q * r;   r_prev = r;   r = r_next;
      Z s_next = s_prev - q * s;   s_prev = s;   s = s_next;
    }
    // Now r_prev holds GCD(a, N).
    if (r_prev > 1) return NoWeight();  // GCD != 1, a and N are not coprime.
    s_prev += N * (s_prev < 0);
    return s_prev;
  }
};

// Integral binary logarithm of N.
template <int64 N> struct Log2    { enum { value = (1 + Log2<N/2>::value) };};
template <>        struct Log2<1> { enum { value = 0 };};

// Signed integer type of size 2^LogBytes bytes.
template <uint8 LogBytes> struct SignedInt {};
template <> struct SignedInt<0> { typedef int8  Type; };
template <> struct SignedInt<1> { typedef int16 Type; };
template <> struct SignedInt<2> { typedef int32 Type; };
template <> struct SignedInt<3> { typedef int64 Type; };

// This type automatically selects the smallest signed integer type that can
// hold all positive remainders modulo N. The size compuation first takes the
// binary logarithm of N-1 to determine the number of bits required to store
// the largest remainder modulo N, rounds that up to fit into bytes, then takes
// another binary logarithm to compute the log-bytes required for the template
// argument Z.
template <int64 N>
using IntegersMod = ModularIntegerSemiring<
  typename SignedInt<Log2<((9+Log2<N-1>::value)/8)*2-1>::value>::Type, N>;

}  // namespace festus

#endif  // FESTUS_MODULAR_INT_SEMIRING_H__

// festus/max-times-semiring.h
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
// The Max-Times semiring over the natural numbers {0, 1, ..., LIMIT}.
//
// This semiring is zero-sum-free, one-product-free, and zero-product-free,
// therefore it does not have any nontrivial additive or multiplicative
// inverses. However, a - b is unambiguously defined for all a > b and can be
// defined (as done below) for all a >= b.
//
// Even without inverses and with 1 - a only defined for trivial cases, the
// Star operation can be defined for all elements. The Star operation must
// satisfy the axiom
//
//   a* == 1 + a a* == min(LIMIT, max(1, a a*))
//
// This is the case for 0* == 1 and a* == LIMIT for all a > 1. The Star axiom
// constrains but does not dictate the definition of 1*. To satisfy the Star
// axiom 1* == max(1, 1*), any choice of 1* >= 1 will do. Note that this
// semiring is k-closed iff 1* == 1.

#ifndef FESTUS_MAX_TIMES_SEMIRING_H__
#define FESTUS_MAX_TIMES_SEMIRING_H__

#include <algorithm>
#include <cinttypes>
#include <limits>
#include <ostream>
#include <sstream>

#include <fst/compat.h>

namespace festus {

template <typename N, int64 LIMIT, int64 ONE_STAR = 1>
struct LimitedMaxTimesSemiring {
  static_assert(std::numeric_limits<N>::is_integer, "integral type expected");
  static_assert(std::numeric_limits<N>::is_signed, "signed type expected");
  static_assert(LIMIT >= 0, "expected LIMIT >= 0");
  static_assert(LIMIT <= std::numeric_limits<N>::max(), "LIMIT too large");
  static_assert(ONE_STAR >= 1, "expected ONE_STAR >= 1");
  static_assert(ONE_STAR <= LIMIT, "expected ONE_STAR <= LIMIT");

  static constexpr N Limit() { return static_cast<N>(LIMIT); }

  typedef N ValueType;

  static string Name() {
    std::ostringstream strm;
    strm << "natural_max_times_up_to_" << LIMIT
         << "_with_one_star_eq_" << ONE_STAR;
    return strm.str();
  }

  static constexpr bool Commutative() { return true; }
  static constexpr bool Idempotent() { return true; }

  static constexpr N NoWeight() { return -1; }
  static constexpr N Zero() { return 0; }
  static constexpr N One() { return 1; }
  static constexpr N Reverse(N a) { return a; }
  static constexpr N Quantize(N a, float) { return a; }

  // Constructs an element of the semiring from the given integer a.
  static constexpr N From(std::intmax_t a) {
    return a < 0 ? NoWeight() : (a > Limit() ? Limit() : a);
  }

  // Computes a + b up to LIMIT.
  static constexpr N OpPlus(N a, N b) {
    return (!Member(a) || !Member(b)) ? NoWeight()
        : std::min(Limit(), std::max(a, b));
  }

  // Returns m such that OpPlus(m, b) == a, or NoWeight() if no such m exists.
  // TODO(C++14): Make this a constexpr function.
  static N OpMinus(N a, N b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    if (a > b) {
      return OpPlus(a, b);
    } else if (a == b) {
      return a / 2;  // Minus is not unique when a == b; any value < a works.
    } else {
      return NoWeight();
    }
  }

  // Computes a * b up to LIMIT.
  // TODO(C++14): Make this a constexpr function.
  static N OpTimes(N a, N b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    if (b == 0) return 0;
    if (a <= Limit() / b) return a * b;
    return Limit();
  }

  // Returns d such that OpTimes(d, b) == a == OpTimes(b, d), or NoWeight() if
  // no such d exists.
  static N OpDivide(N a, N b) {
    if (!Member(a) || !Member(b)) return NoWeight();
    if (a == 0) return 0;  // Includes 0/0, which is not unique.
    if (b == 0) return NoWeight();
    if (a >= Limit()) return Limit();  // Not necessarily unique.
    if (b > Limit()) b = Limit();
    N div = a / b;
    if (div * b == a) return div;
    return NoWeight();
  }

  static constexpr N Reciprocal(N a) { return a == 1 ? 1 : NoWeight(); }

  // Returns s such that
  //
  //   s == OpPlus(One(), OpTimes(a, s)) and
  //   s == OpPlus(One(), OpTimes(s, a)).
  //
  // TODO(C++14): Make this a constexpr function.
  static N OpStar(N a) {
    if (!Member(a)) return NoWeight();
    if (a == 0) return 1;
    if (a == 1) return static_cast<N>(ONE_STAR);
    return Limit();
  }

  static constexpr bool Member(N a) { return a >= 0; }

  static constexpr bool NotZero(N a) { return a != 0; }

  static constexpr bool EqualTo(N a, N b) { return Member(a) && a == b; }

  static constexpr bool ApproxEqualTo(N a, N b, float delta) {
    return EqualTo(a, b);
  }

  static std::ostream &Print(std::ostream &strm, N a) {
    return strm << static_cast<int64>(a);
  }
};

}  // namespace festus

#endif  // FESTUS_MAX_TIMES_SEMIRING_H__

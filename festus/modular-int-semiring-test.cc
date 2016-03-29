// festus/modular-int-semiring-test.cc
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
// Unit test for the (semi)ring of integers modulo N.

#include "festus/modular-int-semiring.h"

#include <type_traits>

#include <gtest/gtest.h>

#include "festus/weight.h"
#include "festus/weight-test-lib.h"

namespace {

template <int64 N>
using Z = festus::IntegersMod<N>;

template <int64 N>
using ModularIntWeight = festus::ValueWeightStatic<Z<N>>;

TEST(ModularIntSemiringTest, ValueTypeSizes) {
  EXPECT_EQ(1, sizeof(Z<2>::ValueType));

  EXPECT_EQ(1, sizeof(Z<(1ULL <<  7) - 1>::ValueType));
  EXPECT_EQ(1, sizeof(Z<(1ULL <<  7)    >::ValueType));
  EXPECT_EQ(2, sizeof(Z<(1ULL <<  7) + 1>::ValueType));

  EXPECT_EQ(2, sizeof(Z<(1ULL << 15) - 1>::ValueType));
  EXPECT_EQ(2, sizeof(Z<(1ULL << 15)    >::ValueType));
  EXPECT_EQ(4, sizeof(Z<(1ULL << 15) + 1>::ValueType));

  EXPECT_EQ(4, sizeof(Z<(1ULL << 31) - 1>::ValueType));
  EXPECT_EQ(4, sizeof(Z<(1ULL << 31)    >::ValueType));
  EXPECT_EQ(8, sizeof(Z<(1ULL << 31) + 1>::ValueType));

  EXPECT_EQ(8, sizeof(Z<(1ULL << 63) - 1>::ValueType));
}

TEST(ModularIntSemiringTest, WeightSizesAndTraits) {
  // These could become compile-time static assertions, but that would risk
  // build breakage instead of test failures.
  EXPECT_EQ(1, sizeof(ModularIntWeight<2>));
  EXPECT_EQ(1, sizeof(ModularIntWeight<(1ULL << 7)>));
  EXPECT_EQ(2, sizeof(ModularIntWeight<(1ULL << 7) + 1>));
  EXPECT_EQ(2, sizeof(ModularIntWeight<(1ULL << 15)>));
  EXPECT_EQ(4, sizeof(ModularIntWeight<(1ULL << 15) + 1>));
  EXPECT_EQ(4, sizeof(ModularIntWeight<(1ULL << 31)>));
  EXPECT_EQ(8, sizeof(ModularIntWeight<(1ULL << 31) + 1>));
  EXPECT_EQ(8, sizeof(ModularIntWeight<(1ULL << 63) - 1>));

  EXPECT_TRUE(std::is_pod<ModularIntWeight<2>>::value);
  EXPECT_TRUE(std::is_pod<ModularIntWeight<(1ULL << 63) - 1>>::value);
}

TEST(ModularIntSemiringTest, NoOverflow) {
  EXPECT_EQ(  0, Z<127>::Remainder(-127));
  EXPECT_EQ(124, Z<127>::OpPlus(  125,  126));
  EXPECT_EQ(124, Z<127>::OpPlus(  126,  125));
  EXPECT_EQ(  3, Z<127>::OpPlus( -125, -126));
  EXPECT_EQ(  3, Z<127>::OpPlus( -126, -125));
  EXPECT_EQ(  0, Z<127>::OpPlus(  127,  127));
  EXPECT_EQ(  0, Z<127>::OpMinus(-127,  127));
  EXPECT_EQ(  0, Z<127>::OpMinus(   0, -127));
  EXPECT_EQ(124, Z<127>::OpMinus( 125, -126));
  EXPECT_EQ(125, Z<127>::OpMinus( 125, -127));
  EXPECT_EQ(  2, Z<127>::OpTimes( 125,  126));
  EXPECT_EQ(  2, Z<127>::OpTimes( 126,  125));
  EXPECT_EQ( 18, Z<127>::OpTimes(  97,  101));
  EXPECT_EQ( 18, Z<127>::OpTimes( 101,   97));
  EXPECT_EQ(109, Z<127>::OpTimes( -97,  101));
  EXPECT_EQ(109, Z<127>::OpTimes( 101,  -97));
  EXPECT_EQ(  0, Z<127>::OpTimes(-127, -127));
  EXPECT_EQ(  0, Z<127>::OpTimes(-127,  127));
  EXPECT_EQ(  0, Z<127>::OpTimes( 127, -127));
  EXPECT_EQ(  0, Z<127>::OpTimes( 127,  127));
}

TEST(ModularIntSemiringTest, Reciprocal) {
  EXPECT_EQ(  1, Z<128>::Reciprocal( 1));
  EXPECT_EQ( 43, Z<128>::Reciprocal( 3));
  EXPECT_EQ( 77, Z<128>::Reciprocal( 5));
  EXPECT_EQ( 55, Z<128>::Reciprocal( 7));
  EXPECT_EQ( 57, Z<128>::Reciprocal( 9));
  EXPECT_EQ( 35, Z<128>::Reciprocal(11));
  EXPECT_EQ( 69, Z<128>::Reciprocal(13));
  EXPECT_EQ(111, Z<128>::Reciprocal(15));
  EXPECT_EQ(113, Z<128>::Reciprocal(17));
  EXPECT_EQ( 27, Z<128>::Reciprocal(19));
  for (int a = 2; a < 128; a += 2) {
    EXPECT_FALSE(Z<128>::Member(Z<128>::Reciprocal(a)));
  }

  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(0)));
  EXPECT_EQ( 1,              Z<10>::Reciprocal(1));
  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(2)));
  EXPECT_EQ( 7,              Z<10>::Reciprocal(3));
  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(4)));
  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(5)));
  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(6)));
  EXPECT_EQ( 3,              Z<10>::Reciprocal(7));
  EXPECT_FALSE(Z<10>::Member(Z<10>::Reciprocal(8)));
  EXPECT_EQ( 9,              Z<10>::Reciprocal(9));

  EXPECT_EQ( 1, Z<13>::Reciprocal( 1));
  EXPECT_EQ( 7, Z<13>::Reciprocal( 2));
  EXPECT_EQ( 9, Z<13>::Reciprocal( 3));
  EXPECT_EQ(10, Z<13>::Reciprocal( 4));
  EXPECT_EQ( 8, Z<13>::Reciprocal( 5));
  EXPECT_EQ(11, Z<13>::Reciprocal( 6));
  EXPECT_EQ( 2, Z<13>::Reciprocal( 7));
  EXPECT_EQ( 5, Z<13>::Reciprocal( 8));
  EXPECT_EQ( 3, Z<13>::Reciprocal( 9));
  EXPECT_EQ( 4, Z<13>::Reciprocal(10));
  EXPECT_EQ( 6, Z<13>::Reciprocal(11));
  EXPECT_EQ(12, Z<13>::Reciprocal(12));
  EXPECT_FALSE(Z<13>::Member(Z<13>::Reciprocal(13)));
}

template <long N>
void ExhaustiveTestZ() {
  typedef Z<N> Ring;
  constexpr auto z = Ring::Zero();
  constexpr auto o = Ring::One();
  EXPECT_NE(o, Ring::OpPlus(o, o));
  EXPECT_EQ(o, Ring::Reciprocal(o));
  EXPECT_EQ(o, Ring::OpStar(z));
  for (int a = 0; a < N; ++a) {
    EXPECT_EQ(a, Ring::OpPlus(a, z));
    EXPECT_EQ(a, Ring::OpPlus(z, a));
    EXPECT_EQ(a, Ring::OpMinus(a, z));
    EXPECT_EQ(z, Ring::OpMinus(a, a));
    EXPECT_EQ(z, Ring::OpTimes(a, z));
    EXPECT_EQ(z, Ring::OpTimes(z, a));
    EXPECT_EQ(a, Ring::OpTimes(a, o));
    EXPECT_EQ(a, Ring::OpTimes(o, a));
    auto s = Ring::OpStar(a);
    if (Ring::Member(s)) {
      EXPECT_EQ(s, Ring::OpPlus(o, Ring::OpTimes(a, s)));
      EXPECT_EQ(s, Ring::OpPlus(o, Ring::OpTimes(s, a)));
    }
    for (int b = 0; b < N; ++b) {
      auto p = Ring::OpPlus(a, b);
      EXPECT_TRUE(Ring::Member(p));
      auto m = Ring::OpMinus(p, a);
      EXPECT_TRUE(Ring::Member(m));
      EXPECT_EQ(b, m);
      auto t = Ring::OpTimes(a, b);
      EXPECT_TRUE(Ring::Member(t));
      auto d = Ring::OpDivide(t, a);
      if (Ring::NotZero(a)) {
        if (Ring::Member(d)) {
          EXPECT_EQ(b, d);
        }
      } else {
        EXPECT_FALSE(Ring::Member(d));
      }
    }
  }
}

TEST(ModularIntSemiringTest, Exhaustive) {
  ExhaustiveTestZ<2>();
  ExhaustiveTestZ<10>();
  ExhaustiveTestZ<13>();
  ExhaustiveTestZ<127>();
  ExhaustiveTestZ<128>();
}

TEST(ModularIntSemiringTest, BasicWeights) {
  festus::TestBasicWeights<ModularIntWeight<2>>();
  festus::TestBasicWeights<ModularIntWeight<10>>();
  festus::TestBasicWeights<ModularIntWeight<13>>();
  festus::TestBasicWeights<ModularIntWeight<127>>();
  festus::TestBasicWeights<ModularIntWeight<128>>();
}

template <int64 N>
void TestModularIntIdentities() {
  typedef ModularIntWeight<N> Weight;
  for (int a = -300; a <= 300; ++a) {
    Weight w = Weight::SemiringType::FromInteger(a);
    TestIdentities(w);
  }
}

TEST(ModularIntSemiringTest, Identities) {
  TestModularIntIdentities<2>();
  TestModularIntIdentities<10>();
  TestModularIntIdentities<13>();
  TestModularIntIdentities<127>();
  TestModularIntIdentities<128>();
}

}  // namespace

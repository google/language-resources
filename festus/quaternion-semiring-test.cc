// festus/quaternion-semiring-test.cc
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
// Unit test for the quaternion semiring.

#include "festus/quaternion-semiring.h"

#include <type_traits>

#include <fst/weight.h>
#include <gtest/gtest.h>

#include "festus/real-weight.h"
#include "festus/weight-test-lib.h"

namespace {

typedef festus::QuaternionWeightTpl<festus::RealSemiring<float>>
QuaternionWeight;

TEST(QuaternionSemiringTest, TypeTraits) {
  EXPECT_TRUE(std::is_pod<QuaternionWeight>::value);
  EXPECT_EQ(16, sizeof(QuaternionWeight));
}

TEST(QuaternionSemiringTest, BasicWeights) {
  festus::TestBasicWeights<QuaternionWeight>();
}

// Additive inverse.
template <class W>
inline W Negative(W w) { return Minus(W::Zero(), w); }

TEST(QuaternionSemiringTest, Identities) {
  const auto one = QuaternionWeight::From(1);
  const auto i = QuaternionWeight::From(0, 1, 0, 0);
  const auto j = QuaternionWeight::From(0, 0, 1, 0);
  const auto k = QuaternionWeight::From(0, 0, 0, 1);

  festus::TestIdentities(one);
  festus::TestIdentities(i);
  festus::TestIdentities(j);
  festus::TestIdentities(k);
  festus::TestIdentities(Negative(one));

  festus::ExpectEq(Negative(one), Times(i, i));
  festus::ExpectEq(Negative(one), Times(j, j));
  festus::ExpectEq(Negative(one), Times(k, k));
  festus::ExpectEq(Negative(one), Times(i, Times(j, k)));
  festus::ExpectEq(Negative(one), Times(Times(i, j), k));

  festus::ExpectEq(k,           Times(i, j));
  festus::ExpectEq(Negative(k), Times(j, i));
  festus::ExpectEq(i,           Times(j, k));
  festus::ExpectEq(Negative(i), Times(k, j));
  festus::ExpectEq(j,           Times(k, i));
  festus::ExpectEq(Negative(j), Times(i, k));

  const auto x = QuaternionWeight::From(0.5, 1.5, -0.5, 2.5);
  const auto y = QuaternionWeight::From(-2, 0, 4, -4);
  festus::TestIdentities(x);
  festus::TestIdentities(y);
  EXPECT_EQ( 9, QuaternionWeight::SemiringType::Norm2(x.Value()));
  EXPECT_EQ(36, QuaternionWeight::SemiringType::Norm2(y.Value()));

  const auto x_reciprocal = QuaternionWeight(
      QuaternionWeight::SemiringType::Reciprocal(x.Value()));
  festus::TestIdentities(x_reciprocal);
  EXPECT_TRUE(ApproxEqual(Times(x, x_reciprocal), one, 1e-12));
  EXPECT_TRUE(ApproxEqual(Times(x_reciprocal, x), one, 1e-12));

  const auto x_div_y = Divide(x, y, fst::DIVIDE_RIGHT);
  const auto y_vid_x = Divide(x, y, fst::DIVIDE_LEFT);
  festus::TestIdentities(x_div_y);
  festus::TestIdentities(y_vid_x);
  EXPECT_TRUE(ApproxEqual(Times(x_div_y, y), x, 1e-6));
  EXPECT_TRUE(ApproxEqual(Times(y, y_vid_x), x, 1e-6));
  EXPECT_FALSE(ApproxEqual(x_div_y, y_vid_x, 0.1));
}

}  // namespace

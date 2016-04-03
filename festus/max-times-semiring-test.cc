// festus/max-times-semiring-test.cc
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
// Unit test for the Max-Times semiring over {0, 1, ..., LIMIT}.

#include "festus/max-times-semiring.h"

#include <gtest/gtest.h>

#include "festus/weight.h"
#include "festus/weight-test-lib.h"

namespace {

template <int8 LIMIT, int8 ONE_STAR>
using MaxTimesWeight = festus::ValueWeightStatic<
  festus::LimitedMaxTimesSemiring<int8, LIMIT, ONE_STAR>>;

TEST(MaxTimesSemiringTest, BasicWeights) {
  festus::TestBasicWeights<MaxTimesWeight<1, 1>>();
  festus::TestBasicWeights<MaxTimesWeight<2, 1>>();
  festus::TestBasicWeights<MaxTimesWeight<2, 2>>();
  festus::TestBasicWeights<MaxTimesWeight<3, 1>>();
  festus::TestBasicWeights<MaxTimesWeight<3, 2>>();
  festus::TestBasicWeights<MaxTimesWeight<3, 3>>();
}

template <int8 LIMIT, int8 ONE_STAR>
void TestMaxTimesIdentities() {
  typedef MaxTimesWeight<LIMIT, ONE_STAR> Weight;
  for (int a = 0; a - 1 <= LIMIT; ++a) {
    Weight w = Weight::From(a);
    TestIdentities(w);
  }
}

TEST(MaxTimesSemiringTest, Identities) {
  TestMaxTimesIdentities<1, 1>();
  TestMaxTimesIdentities<2, 1>();
  TestMaxTimesIdentities<2, 2>();
  TestMaxTimesIdentities<3, 1>();
  TestMaxTimesIdentities<3, 2>();
  TestMaxTimesIdentities<3, 3>();
  TestMaxTimesIdentities<25, 1>();
  TestMaxTimesIdentities<25, 2>();
  TestMaxTimesIdentities<25, 25>();
}

TEST(MaxTimesSemiringTest, MaxTimes_3_2) {
  typedef MaxTimesWeight<3, 2> Weight;
  const Weight w0 = Weight::From(0);
  const Weight w1 = Weight::From(1);
  const Weight w2 = Weight::From(2);
  const Weight w3 = Weight::From(3);

  festus::ExpectEq(w0, Weight::Zero());
  festus::ExpectEq(w1, Weight::One());
  festus::ExpectEq(w3, Weight::From(4));

  festus::ExpectEq(w0, Plus(w0, w0));
  festus::ExpectEq(w1, Plus(w0, w1));
  festus::ExpectEq(w2, Plus(w0, w2));
  festus::ExpectEq(w3, Plus(w0, w3));

  festus::ExpectEq(w1, Plus(w1, w0));
  festus::ExpectEq(w1, Plus(w1, w1));
  festus::ExpectEq(w2, Plus(w1, w2));
  festus::ExpectEq(w3, Plus(w1, w3));

  festus::ExpectEq(w2, Plus(w2, w0));
  festus::ExpectEq(w2, Plus(w2, w1));
  festus::ExpectEq(w2, Plus(w2, w2));
  festus::ExpectEq(w3, Plus(w2, w3));

  festus::ExpectEq(w3, Plus(w3, w0));
  festus::ExpectEq(w3, Plus(w3, w1));
  festus::ExpectEq(w3, Plus(w3, w2));
  festus::ExpectEq(w3, Plus(w3, w3));

  festus::ExpectEq(w0, Minus(w0, w0));
  EXPECT_FALSE(Minus(w0, w1).Member());
  EXPECT_FALSE(Minus(w0, w2).Member());
  EXPECT_FALSE(Minus(w0, w3).Member());

  festus::ExpectEq(w1, Minus(w1, w0));
  festus::ExpectEq(w0, Minus(w1, w1));
  EXPECT_FALSE(Minus(w1, w2).Member());
  EXPECT_FALSE(Minus(w1, w3).Member());

  festus::ExpectEq(w2, Minus(w2, w0));
  festus::ExpectEq(w2, Minus(w2, w1));
  festus::ExpectEq(w1, Minus(w2, w2));
  EXPECT_FALSE(Minus(w2, w3).Member());

  festus::ExpectEq(w3, Minus(w3, w0));
  festus::ExpectEq(w3, Minus(w3, w1));
  festus::ExpectEq(w3, Minus(w3, w2));
  festus::ExpectEq(w1, Minus(w3, w3));

  festus::ExpectEq(w0, Times(w0, w0));
  festus::ExpectEq(w0, Times(w0, w1));
  festus::ExpectEq(w0, Times(w0, w2));
  festus::ExpectEq(w0, Times(w0, w3));

  festus::ExpectEq(w0, Times(w1, w0));
  festus::ExpectEq(w1, Times(w1, w1));
  festus::ExpectEq(w2, Times(w1, w2));
  festus::ExpectEq(w3, Times(w1, w3));

  festus::ExpectEq(w0, Times(w2, w0));
  festus::ExpectEq(w2, Times(w2, w1));
  festus::ExpectEq(w3, Times(w2, w2));
  festus::ExpectEq(w3, Times(w2, w3));

  festus::ExpectEq(w0, Times(w3, w0));
  festus::ExpectEq(w3, Times(w3, w1));
  festus::ExpectEq(w3, Times(w3, w2));
  festus::ExpectEq(w3, Times(w3, w3));

  festus::ExpectEq(w0, Divide(w0, w0));
  festus::ExpectEq(w0, Divide(w0, w1));
  festus::ExpectEq(w0, Divide(w0, w2));
  festus::ExpectEq(w0, Divide(w0, w3));

  EXPECT_FALSE(Divide(w1, w0).Member());
  festus::ExpectEq(w1, Divide(w1, w1));
  EXPECT_FALSE(Divide(w1, w2).Member());
  EXPECT_FALSE(Divide(w1, w3).Member());

  EXPECT_FALSE(Divide(w2, w0).Member());
  festus::ExpectEq(w2, Divide(w2, w1));
  festus::ExpectEq(w1, Divide(w2, w2));
  EXPECT_FALSE(Divide(w2, w3).Member());

  EXPECT_FALSE(Divide(w3, w0).Member());
  festus::ExpectEq(w3, Divide(w3, w1));
  festus::ExpectEq(w3, Divide(w3, w2));
  festus::ExpectEq(w3, Divide(w3, w3));

  festus::ExpectEq(w1, Star(w0));
  festus::ExpectEq(w2, Star(w1));
  festus::ExpectEq(w3, Star(w2));
  festus::ExpectEq(w3, Star(w3));
}

}  // namespace

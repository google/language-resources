// festus/algebraic-path-test.cc
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
// Unit test for algebraic path computation.

#include "festus/algebraic-path.h"

#include <fst/fstlib.h>
#include <gtest/gtest.h>

#include "festus/arc.h"
#include "festus/float-weight-star.h"
#include "festus/max-times-semiring.h"
#include "festus/modular-int-semiring.h"
#include "festus/value-weight-static.h"

namespace {

TEST(AlgebraicPathTest, IntegersMod13) {
  // Note that this semiring is not k-closed.
  typedef festus::IntegersMod<13> SemiringType;
  typedef festus::ValueWeightStatic<SemiringType> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  // Internal implementation detail:
  typedef typename festus::internal::SemiringFor<Weight> SemiringForWeight;
  EXPECT_EQ(1, SemiringForWeight::IsSpecialized());

  const auto &semiring = SemiringForWeight::Instance();
  EXPECT_EQ(semiring.Zero(), Weight::Zero().Value());

  fst::VectorFst<Arc> fst;
  auto s = fst.AddState();
  fst.SetStart(s);
  fst.AddArc(s, Arc(0, 0, Weight::From(3), s));
  fst.SetFinal(s, Weight::From(11));

  const auto total_value = festus::SumTotalValue(fst, &semiring);
  EXPECT_EQ(1, total_value);

  const Weight total_weight = festus::SumTotalWeight(fst);
  EXPECT_EQ(Weight::One(), total_weight);
}

TEST(AlgebraicPathTest, LimitedMaxTimes) {
  // Note that this semiring is not k-closed.
  typedef festus::LimitedMaxTimesSemiring<int8, 3, 2> SemiringType;
  typedef festus::ValueWeightStatic<SemiringType> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  // Internal implementation detail:
  typedef typename festus::internal::SemiringFor<Weight> SemiringForWeight;
  EXPECT_EQ(1, SemiringForWeight::IsSpecialized());

  const auto &semiring = SemiringForWeight::Instance();
  EXPECT_EQ(semiring.Zero(), Weight::Zero().Value());

  fst::VectorFst<Arc> fst;
  auto s = fst.AddState();
  fst.SetStart(s);
  fst.AddArc(s, Arc(0, 0, Weight::From(1), s));
  fst.SetFinal(s, Weight::From(1));

  const auto total_value = festus::SumTotalValue(fst, &semiring);
  EXPECT_EQ(2, total_value);

  const Weight total_weight = festus::SumTotalWeight(fst);
  EXPECT_EQ(Weight::From(2), total_weight);
}

TEST(AlgebraicPathTest, Log) {
  typedef fst::LogArc Arc;
  typedef Arc::Weight Weight;

  // Internal implementation detail:
  typedef typename festus::internal::SemiringFor<Weight> SemiringForWeight;
  EXPECT_EQ(0, SemiringForWeight::IsSpecialized());

  const auto &semiring = SemiringForWeight::Instance();
  EXPECT_EQ(semiring.Zero(), Weight::Zero().Value());

  fst::VectorFst<Arc> fst;
  auto s = fst.AddState();
  fst.SetStart(s);
  fst.AddArc(s, Arc(0, 0, Weight(9.53674771e-07f), s));
  fst.SetFinal(s, Weight(13.8629436f));

  const auto total_value = festus::SumTotalValue(fst, &semiring);
  EXPECT_NEAR(0, total_value, 1e-12);

  const Weight total_weight = festus::SumTotalWeight(fst);
  EXPECT_TRUE(ApproxEqual(Weight::One(), total_weight));
}

}  // namespace

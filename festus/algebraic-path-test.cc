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

#include <cmath>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <gtest/gtest.h>

#include "festus/arc.h"
#include "festus/float-weight-star.h"
#include "festus/max-times-semiring.h"
#include "festus/modular-int-semiring.h"
#include "festus/quaternion-semiring.h"
#include "festus/real-weight.h"
#include "festus/value-weight-static.h"

DEFINE_double(delta, fst::kDelta,
              "Convergence threshold for ShortestDistance");
DEFINE_bool(modular_int, false,
            "Try to compute ShortestDistance in modular int semiring");
DEFINE_bool(quaternion, false,
            "Try to compute ShortestDistance in quaternion semiring");

namespace {

TEST(AlgebraicPathTest, Log) {
  typedef fst::LogArc Arc;
  typedef Arc::Weight Weight;

  // Internal implementation detail:
  typedef typename festus::internal::SemiringFor<Weight> SemiringForWeight;
  EXPECT_EQ(0, SemiringForWeight::IsSpecialized());

  const auto &semiring = SemiringForWeight::Instance();
  EXPECT_EQ(semiring.Zero(), Weight::Zero().Value());

  constexpr float q = 1.0f / 32768.0f;

  fst::VectorFst<Arc> fst;
  auto s = fst.AddState();
  fst.SetStart(s);
  fst.AddArc(s, Arc(0, 0, -std::log1p(-q), s));
  fst.SetFinal(s, -std::log(q));

  const auto total_value = festus::SumTotalValue(fst, &semiring);
  EXPECT_NEAR(0, total_value, 1e-9);

  const Weight total_weight = festus::SumTotalWeight(fst);
  EXPECT_TRUE(ApproxEqual(Weight::One(), total_weight));

  VLOG(0) << "sum total = " << total_weight;
  VLOG(0) << "shortest distance computation will be slow and imprecise:";
  VLOG(0) << "shortest distance = " << fst::ShortestDistance(fst, FLAGS_delta);
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

  VLOG(0) << "sum total = " << total_weight;
  VLOG(0) << "shortest distance computation will be fast and wrong:";
  VLOG(0) << "shortest distance =  " << fst::ShortestDistance(fst, FLAGS_delta);
}

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

  VLOG(0) << "sum total = " << total_weight;
  if (FLAGS_modular_int) {
    VLOG(0) << "shortest distance computation will not terminate:";
    VLOG(0) << "shortest distance = "
            << fst::ShortestDistance(fst, FLAGS_delta);
  }
}

TEST(AlgebraicPathTest, Quaternion) {
  // Note that this semiring is not k-closed.
  typedef festus::QuaternionWeightTpl<festus::RealSemiring<float>> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  // Internal implementation detail:
  typedef typename festus::internal::SemiringFor<Weight> SemiringForWeight;
  EXPECT_EQ(1, SemiringForWeight::IsSpecialized());

  const auto &semiring = SemiringForWeight::Instance();
  EXPECT_EQ(semiring.Zero(), Weight::Zero().Value());

  const Weight p = Weight::From(-0.5f, 0.5f, 0.5f, 0.5f);
  const Weight q = Minus(Weight::One(), p);

  fst::VectorFst<Arc> fst;
  auto s = fst.AddState();
  fst.SetStart(s);
  fst.AddArc(s, Arc(0, 0, p, s));
  fst.SetFinal(s, q);

  const auto total_value = festus::SumTotalValue(fst, &semiring);
  EXPECT_NEAR(1, total_value[0], 1e-7);
  EXPECT_NEAR(0, total_value[1], 1e-7);
  EXPECT_NEAR(0, total_value[2], 1e-7);
  EXPECT_NEAR(0, total_value[3], 1e-7);

  const Weight total_weight = festus::SumTotalWeight(fst);
  EXPECT_TRUE(ApproxEqual(Weight::One(), total_weight, 1e-7));

  VLOG(0) << "sum total = " << total_weight;
  if (FLAGS_quaternion) {
    VLOG(0) << "shortest distance computation will not terminate:";
    VLOG(0) << "shortest distance = "
            << fst::ShortestDistance(fst, FLAGS_delta);
  }
}

}  // namespace

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  SET_FLAGS(argv[0], &argc, &argv, true);
  return RUN_ALL_TESTS();
}

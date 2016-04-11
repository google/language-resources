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

template <class Arc>
void TestOneStateLoop(
    typename Arc::Weight loop_weight,
    typename Arc::Weight final_weight,
    typename Arc::Weight expected_sum_total,
    float comparison_delta,
    int power_series_terms,
    bool use_shortest_distance,
    const char *msg) {
  typedef typename Arc::Weight Weight;

  // One-state FST with a loop at its only (initial and final) state.
  fst::VectorFst<Arc> fst;
  const auto state = fst.AddState();
  fst.SetStart(state);
  fst.AddArc(state, Arc(0, 0, loop_weight, state));
  fst.SetFinal(state, final_weight);

  const Weight sum_total = festus::SumTotalWeight(fst);
  EXPECT_TRUE(ApproxEqual(sum_total, expected_sum_total, comparison_delta));

  VLOG(0) << "sum total = " << sum_total << " ~= " << expected_sum_total;

  if (power_series_terms) {
    Weight power = Weight::One();
    Weight series = Weight::One();
    VLOG(0) << "\\sum_{n=0}^0 loop_weight^n = " << series;
    VLOG(0) << "  sum x final = " << Times(series, final_weight);
    for (int n = 1; n <= power_series_terms; ++n) {
      power = Times(power, loop_weight);
      series = Plus(series, power);
      VLOG(0) << "\\sum_{n=0}^" << n << " loop_weight^n = " << series;
      VLOG(0) << "  sum x final = " << Times(series, final_weight);
    }
  }

  if (use_shortest_distance) {
    VLOG(0) << msg;
    VLOG(0) << "shortest distance = "
            << fst::ShortestDistance(fst, FLAGS_delta);
  }
}

TEST(AlgebraicPathTest, Log) {
  typedef fst::LogArc Arc;
  typedef Arc::Weight Weight;

  constexpr float q = 1.0f / 32768.0f;

  TestOneStateLoop<Arc>(
      Weight(-std::log1p(-q)), Weight(-std::log(q)), Weight::One(), 1e-9, 9,
      true, "shortest distance computation will be slow and imprecise");

  // Internal implementation detail:
  EXPECT_EQ(0, festus::internal::SemiringFor<Weight>::IsSpecialized());
}

TEST(AlgebraicPathTest, LimitedMaxTimes) {
  // Note that this semiring is not k-closed.
  typedef festus::LimitedMaxTimesSemiring<int8, 3, 2> SemiringType;
  typedef festus::ValueWeightStatic<SemiringType> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  TestOneStateLoop<Arc>(
      Weight::From(1), Weight::From(1), Weight::From(2), 1e-30, 3,
      true, "shortest distance computation will be fast and different");

  // Internal implementation detail:
  EXPECT_EQ(1, festus::internal::SemiringFor<Weight>::IsSpecialized());
}

TEST(AlgebraicPathTest, IntegersMod13) {
  // Note that this semiring is not k-closed.
  typedef festus::IntegersMod<13> SemiringType;
  typedef festus::ValueWeightStatic<SemiringType> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  TestOneStateLoop<Arc>(
      Weight::From(3), Weight::From(11), Weight::One(), 1e-30, 5,
      FLAGS_modular_int, "shortest distance computation will not terminate");

  // Internal implementation detail:
  EXPECT_EQ(1, festus::internal::SemiringFor<Weight>::IsSpecialized());
}

TEST(AlgebraicPathTest, Quaternion) {
  // Note that this semiring is not k-closed.
  typedef festus::QuaternionWeightTpl<festus::RealSemiring<float>> Weight;
  typedef festus::ValueArcTpl<Weight> Arc;

  const Weight p = Weight::From(-0.5f, 0.5f, 0.5f, 0.5f);
  const Weight q = Minus(Weight::One(), p);

  TestOneStateLoop<Arc>(
      p, q, Weight::One(), 1e-9, 5,
      FLAGS_quaternion, "shortest distance computation will not terminate");

  // Internal implementation detail:
  EXPECT_EQ(1, festus::internal::SemiringFor<Weight>::IsSpecialized());
}

}  // namespace

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  SET_FLAGS(argv[0], &argc, &argv, true);
  return RUN_ALL_TESTS();
}

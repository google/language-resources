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
//
// This also illustrates the different behaviors of fst::ShortestDistance(fst)
// vs. festus::SumTotalWeight(fst):
//
// * ShortestDistance() requires, but cannot check, that the weight semiring
//   is (approximately) k-closed. It uses this assumption to compute an
//   implicitly defined Star() closure of the semiring by expanding out a
//   power series until (approximate) convergence.
//
// * SumTotalWeight() requires that a Star() operation has been explicitly
//   defined which satisfies the Star axiom. It makes no assumptions about
//   Star() being equivalent to a convergent power series.
//
// The behavior of these two functions differs in at least the following
// situations:
//
// * The power series for all observed cycles converge; the limit of a
//   convergent power series is also provided by the corresponding Star()
//   operation. This means that, for all practical purposes and with only minor
//   hand-waving, the semiring is approximately k-closed. This is the case,
//   inter alia, for the log semiring and graphs/FSTs without negative weight
//   cycles, i.e. all real weights fall into [0;1), so the geometric power
//   series converges.
//
//   In this situation SumTotalWeight() and ShortestDistance() will eventually
//   give approximately the same answer. However, there are differences in
//   terms of speed and quality.
//
//   SumTotalWeight() will give a generally more precise answer in terms of
//   Star(), which is quick to compute by itself, but uses an algorithm with
//   worst-case cubic running time in the number of states/vertices (TODO:
//   running time dominated by the sum of the cube of the strongly connected
//   component sizes).
//
//   ShortestDistance() will give an answer in terms of the limit of a
//   convergent power series, but in order to do so it has to expand out the
//   terms of the power series one-by-one (without acceleration) until
//   approximate convergence. For high-probability cycles (whose real
//   probabilities are close to 1), convergence can be slow. The quality of the
//   answer also depends on the configured convergence threshold.
//
// * A cycle weight can be computed via a convergent power series, but its
//   limit differs from the result of the defined Star() operation. This is
//   the case for the particular instance of LimitedMaxTimesSemiring below,
//   where 2 == Star(1) != max(1^0, 1^1, 1^2, 1^3, ...) == 1.
//
//   In this situation SumTotalWeight() will give an answer in terms of Star(),
//   and ShortestDistance() will give a different answer in terms of the limit
//   of the convergent power series.
//
// * The power series for a cycle weight diverges, but the corresponding Star()
//   value is well-defined. This is the case, inter alia, for the finite field
//   of integers modulo a prime, and for the division ring of quaternions. In
//   all division rings (and hence all fields), whenever Star(x) is defined, it
//   must be defined as Star(x) := (1 - x)^{-1}; and in all rings, Star(x) must
//   be undefined when 1 - x == 0.
//
//   In this situation SumTotalWeight() will give an answer in terms of Star(),
//   and ShortestDistance() will not terminate.

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

DEFINE_double(delta, 1e-16,
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
    double comparison_delta,
    int power_series_terms,
    bool use_shortest_distance,
    const char *msg) {
  typedef typename Arc::Weight Weight;

  VLOG(0) << "loop weight = " << loop_weight;

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
    VLOG(0) << "  sum * final = " << Times(series, final_weight);
    for (int n = 1; n <= power_series_terms; ++n) {
      power = Times(power, loop_weight);
      series = Plus(series, power);
      VLOG(0) << "\\sum_{n=0}^" << n << " loop_weight^n = " << series;
      VLOG(0) << "  sum * final = " << Times(series, final_weight);
    }
  }

  if (use_shortest_distance) {
    VLOG(0) << msg;
    VLOG(0) << "shortest distance = "
            << fst::ShortestDistance(fst, FLAGS_delta);
  }
}

TEST(AlgebraicPathTest, Log) {
  typedef fst::Log64Arc Arc;
  typedef Arc::Weight Weight;

  constexpr double q = 1.0 / 65536.0;

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

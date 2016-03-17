// festus/matrix-test.cc
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
// Copyright 2015, 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Unit test for matrix semiring operations.

#include "festus/matrix.h"

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

#include <fst/arc.h>
#include <fst/compat.h>
#include <fst/shortest-distance.h>
#include <fst/vector-fst.h>
#include <gtest/gtest.h>

#include "festus/float-weight-star.h"
#include "festus/real-weight.h"

namespace {

// Test that we can compute the inverse of a real matrix M (passed in as mat)
// via M^{-1} == (I - M)*. For easier comprehension, the invese is passed in as
// an unscaled matrix (inv) and determinant (det), i.e. M^{-1} == 1/det * mat.
//
// This illustrates how the MatrixSemiring<RealWeightTpl<T>>::Star() operation
// instantiates the Gauss-Jordan matrix inversion algorithm -- kind of. An
// important aspect of Gauss-Jordan is selecting an appropriate pivot. In
// particular the pivot element cannot be zero. The same is true here, mutatis
// mutandis: the intermediate diagonal element whose Star() we compute must
// never be 1, since then the Star operation of the real semiring would not be
// defined. An intermediate 1 on the diagonal can happen even for well-behaved
// (non-singular, well-conditioned) matrices, and can only be avoided by pivot
// selection. Since this is not a matrix library (use e.g. BLAS/LAPACK instead),
// pivot selection has been deliberately omitted. Nevertheless, the implemented
// functionality can be used to illustrate how the MatrixSemiring::Star
// operation generalizes the core insight of Gauss-Jordan when instantiated to
// the real semiring.
void TestRealMatrixInverse(std::size_t size,
                           std::initializer_list<double> mat,
                           double det,
                           std::initializer_list<double> inv) {
  typedef fst::Real64Weight R;
  typedef festus::MatrixSemiring<R> Semiring;

  Semiring::Matrix matrix;
  Semiring::Matrix inverse;
  auto m_iter = mat.begin();
  auto i_iter = inv.begin();
  for (std::size_t i = 0; i < size; ++i) {
    matrix.emplace_back(std::vector<R>(m_iter, m_iter + size));
    inverse.emplace_back(std::vector<R>(i_iter, i_iter + size));
    m_iter += size;
    i_iter += size;
  }
  EXPECT_EQ(mat.end(), m_iter);
  EXPECT_EQ(inv.end(), i_iter);
  Semiring::Scale(&inverse, 1.0 / det);

  Semiring::Matrix tmp = matrix;
  Semiring::Scale(&tmp, -1);
  Semiring::Matrix m = Semiring::One(size);
  EXPECT_TRUE(Semiring::MPlus(&m, tmp));

  Semiring::MStar(&m);  // m = (I - matrix)* == matrix^{-1}
  Semiring::Matrix mn = Semiring::Zero(size);
  Semiring::Matrix nm = Semiring::Zero(size);
  EXPECT_TRUE(Semiring::MTimes(&mn, m, matrix));
  EXPECT_TRUE(Semiring::MTimes(&nm, matrix, m));
  for (std::size_t i = 0; i < size; ++i) {
    for (std::size_t j = 0; j < size; ++j) {
      EXPECT_DOUBLE_EQ(inverse[i][j].Value(), m[i][j].Value());
      EXPECT_NEAR(i == j ? 1 : 0, mn[i][j].Value(), 1e-15);
      EXPECT_NEAR(i == j ? 1 : 0, nm[i][j].Value(), 1e-15);
    }
  }
}

TEST(MatrixTest, RealMatrixInverse) {
  // The inverse of [1, 2; 3, 4] is 1/-2 * [4, -2; -3, 1].
  TestRealMatrixInverse(2,  { 1,  2,
                              3,  4},
                        -2, { 4, -2,
                             -3,  1});

  // The following requires computing Star(3), Star(5), and Star(-1.25) in the
  // real semiring. Note that Star is well-defined for these inputs, even though
  // the geometric power series would diverge.
  TestRealMatrixInverse(3,  {-2, -3,  2,
                              2, -1,  0,
                             -1,  3,  1},
                        18, {-1,  9,  2,
                             -2,  0,  4,
                              5,  9,  8});
}

template <class W>
void ExpectMatrixEq(const std::vector<W> &expected,
                    const typename festus::MatrixSemiring<W>::Matrix &actual) {
  auto iter = expected.begin();
  for (std::size_t i = 0; i < actual.size(); ++i) {
    auto &actual_i = actual[i];
    for (std::size_t j = 0; j < actual_i.size(); ++j) {
      EXPECT_FLOAT_EQ(iter->Value(), actual_i[j].Value())
          << "i = " << i << "; j = " << j;
      ++iter;
    }
  }
  EXPECT_EQ(expected.end(), iter);
}

// The vertex labels in the Cormen et al. example graph are indexed starting
// from 1. Add corresponding epsilon-arcs (since the arc labels don't matter)
// and offset the vertex indices to produce 0-indexed states.
inline void AddArc(fst::StdVectorFst *fst,
                   int source, float cost, int target) {
  fst->AddArc(source - 1, fst::StdArc(0, 0, cost, target - 1));
}

TEST(MatrixTest, AllPairsDistance) {
  typedef fst::StdArc::Weight Weight;
  constexpr float inf = std::numeric_limits<float>::infinity();

  fst::StdVectorFst fst;
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(i, fst.AddState());
  }
  // This is the graph from Figure 25.1 in Cormen et al., "Introduction to
  // Algorithms" (2001, 2nd edn.; 2009, 3rd edn., p. 690).
  AddArc(&fst, 1,  3, 2);
  AddArc(&fst, 1,  8, 3);
  AddArc(&fst, 1, -4, 5);
  AddArc(&fst, 2,  1, 4);
  AddArc(&fst, 2,  7, 5);
  AddArc(&fst, 3,  4, 2);
  AddArc(&fst, 4,  2, 1);
  AddArc(&fst, 4, -5, 3);
  AddArc(&fst, 5,  6, 4);
  // Add a start state and final state with distinct cost to make this a
  // non-trivial FST.
  fst.SetStart(0);
  fst.SetFinal(0, 9);

  auto matrix = festus::AdjacencyMatrix(fst);
  ExpectMatrixEq(std::vector<Weight>{
      inf,   3,   8, inf, -4,    9,
      inf, inf, inf,   1,  7,  inf,
      inf,   4, inf, inf, inf, inf,
      2.0, inf,  -5, inf, inf, inf,
      inf, inf, inf,   6, inf, inf,
      inf, inf, inf, inf, inf, inf}, matrix);

  auto distance = festus::AllPairsDistance(fst);
  ExpectMatrixEq(std::vector<Weight>{
      0.0,   1,  -3,   2,  -4,   9,
      3.0,   0,  -4,   1,  -1,  12,
      7.0,   4,   0,   5,   3,  16,
      2.0,  -1,  -5,   0,  -2,  11,
      8.0,   5,   1,   6,   0,  17,
      inf, inf, inf, inf, inf,   0}, distance);

  std::vector<Weight> distance_vector;
  fst::ShortestDistance(fst, &distance_vector);
  // The forward distance_vector is equal to the first row of the all-pairs
  // distance matrix (but omitting the last entry for the super-final state):
  ExpectMatrixEq(std::vector<Weight>{0, 1, -3, 2, -4},
                 {distance_vector});

  fst::ShortestDistance(fst, &distance_vector, true);
  // The backward distance vector is equal to the last column of the all-pairs
  // distance matrix (but omitting the last entry for the super-final state):
  ExpectMatrixEq(std::vector<Weight>{9, 12, 16, 11, 17},
                 {distance_vector});
}

// This is an example where computing the total marginal probability of a
// stochastic FST is faster and/or more precise when using the exact
// AllPairsDistance algorithm compared with the ShortestDistance algorithm,
// which is approximate for cyclic FSTs over at most approximately k-closed
// semirings like the log semiring.
//
// In the case of a high-probability cycle, the ShortestDistance algorithm has
// to traverse the cycle many times to reach the desired convergence. If the
// convergence threshold is too lax, the result of ShortestDistance is very far
// from the true answer.
//
// By contrast, the AllPairsDistance algorithm examines the possibly cyclic set
// of paths through each of the states of the FST once per state, i.e. it
// carries out fst.NumStates() Star operations in the FST's Log64Weight
// semiring. Each Star operation computes the probability of a possibly infinite
// path set as the sum of a convergent geometric series. Because this infinite
// sum can be evaluated in closed form, the Star operation is exact and so is
// the AllPairsDistance algorithm (within the limitations of floating point
// arithmetic).
double HighProbabilityCycle(fst::VectorFst<fst::Log64Arc> *fst) {
  constexpr double lp = 1e-6;
  constexpr double lq = 20;

  EXPECT_EQ(0, fst->AddState());
  fst->AddArc(0, {0, 0, lp, 0});
  fst->SetStart(0);
  fst->SetFinal(0, lq);

  const double p = std::exp(-lp);
  const double q = std::exp(-lq);
  EXPECT_LT(p + q, 1);

  const double marginal_probability =
      // -std::log(1.0 / (1.0 - p) * q)
      // -std::log(1.0 / (1.0 - p) - std::log(q)
      //  std::log(1.0 - p) + lq
      std::log1p(-p) + lq;

  return marginal_probability;
}

void LogResult(double expected, double actual) {
  double abs_err = std::abs(actual - expected);
  double rel_err = abs_err / expected;
  std::cout << std::setprecision(15)
            <<   "Computed log probability " << actual
            << "\n   differs from expected " << expected
            << "\n           absolutely by " << abs_err
            << "\n       and relatively by " << rel_err
            << std::endl;
}

TEST(MatrixTest, MarginalProbability_AllPairsDistance) {
  fst::VectorFst<fst::Log64Arc> fst;
  const double expected = HighProbabilityCycle(&fst);

  auto distance = festus::AllPairsDistance(fst);
  auto prob = distance[fst.Start()][fst.NumStates()];

  LogResult(expected, prob.Value());
  EXPECT_NEAR(expected, prob.Value(), 1e-15);
}

TEST(MatrixTest, MarginalProbability_SingleSourceDistance) {
  fst::VectorFst<fst::Log64Arc> fst;
  const double expected = HighProbabilityCycle(&fst);

  // Note that the convergence delta has been tightened in order to achieve the
  // desired absolute error in EXPECT_NEAR below. The absolute error is quite
  // large compared with the all-pairs algorithm and the running time is
  // considerably longer. The elapsed time is printed by the gtest framework as
  // part of its default output. A typical test run looks like this:
  //
  // [ RUN      ] MatrixTest.MarginalProbability_AllPairsDistance
  // Computed log probability 6.18448894201991
  //    differs from expected 6.18448894201991
  //            absolutely by 0
  //        and relatively by 0
  // [       OK ] MatrixTest.MarginalProbability_AllPairsDistance (0 ms)
  // [ RUN      ] MatrixTest.MarginalProbability_SingleSourceDistance
  // Computed log probability 6.18548844338431
  //    differs from expected 6.18448894201991
  //            absolutely by 0.000999501364402633
  //        and relatively by 0.00016161422128377
  // [       OK ] MatrixTest.MarginalProbability_SingleSourceDistance (3913 ms)
  //
  auto prob = fst::ShortestDistance(fst, 1e-9);

  LogResult(expected, prob.Value());
  EXPECT_NEAR(expected, prob.Value(), 1e-3);
}

}  // namespace

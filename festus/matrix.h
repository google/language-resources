// festus/matrix.h
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
// Copyright 2015 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Operations on square matrices over star semirings, including the all-pairs
// distance (algebraic path) computation for weighted graph or FST with weights
// from a star semiring.

#ifndef FESTUS_MATRIX_H__
#define FESTUS_MATRIX_H__

#include <cstddef>
#include <vector>

#include <fst/arc.h>
#include <fst/arcfilter.h>
#include <fst/compat.h>
#include <fst/expanded-fst.h>
#include <fst/fst.h>

#include "festus/float-weight-star.h"
#include "festus/real-weight.h"

namespace fst {

// The square matrices of fixed dimension with entries in a (star) semiring form
// a (star) semiring under the usual matrix addition and multiplication. Mainly
// for efficiency reasons, this class (or more specifically its Matrix type) has
// deliberately not been designed to be usable as an FST Weight class.
template <class W>
class MatrixSemiring {
 public:
  typedef std::vector<std::vector<W>> Matrix;

  explicit MatrixSemiring(std::size_t size) : size_(size) {}

  std::size_t size() const { return size_; }

  Matrix Zero() const { return Diagonal(size_, W::Zero()); }

  Matrix One() const { return Diagonal(size_, W::One()); }

  // Returns a size x size diagonal matrix with the given weight for each of
  // its diagonal values.
  static Matrix Diagonal(std::size_t size, const W &diag);

  // Returns the size x size matrix consisting of Zero elements.
  static Matrix Zero(std::size_t size) { return Diagonal(size, W::Zero()); }

  // Returns the size x size identity matrix consisting of One elements along
  // the diagonal and Zero elements everywhere else.
  static Matrix One(std::size_t size) { return Diagonal(size, W::One()); }

  // Scales m by multiplying each element with w on the right: m := m . w.
  static void Scale(Matrix *m, const W &w);

  // Computes the matrix addition m := m + n.
  static bool Plus(Matrix *m, const Matrix &n);

  // Computes the matrix product: p := p + m n.
  static bool Times(Matrix *p, const Matrix &m, const Matrix &n);

  // Computes the matrix star/asteration/closure see comments below: m := m*.
  static void Star(Matrix *m);

 private:
  const std::size_t size_;
};

template <class W>
typename MatrixSemiring<W>::Matrix MatrixSemiring<W>::Diagonal(
    std::size_t size, const W &diag) {
  Matrix m;
  m.reserve(size);
  for (std::size_t i = 0; i < size; ++i) {
    m.emplace_back(size, W::Zero());
    m.back()[i] = diag;
  }
  return m;
}

template <class W>
void MatrixSemiring<W>::Scale(Matrix *m, const W &w) {
  std::size_t size = m->size();
  for (std::size_t i = 0; i < size; ++i) {
    auto &m_i = (*m)[i];
    DCHECK_EQ(size, m_i.size());
    for (std::size_t j = 0; j < size; ++j) {
      using ::fst::Times;
      m_i[j] = Times(m_i[j], w);
    }
  }
}

template <class W>
bool MatrixSemiring<W>::Plus(Matrix *m, const Matrix &n) {
  std::size_t size = m->size();
  if (n.size() != size) {
    return false;
  }
  for (std::size_t i = 0; i < size; ++i) {
    auto &m_i = (*m)[i];
    const auto &n_i = n[i];
    DCHECK_EQ(size, m_i.size());
    DCHECK_EQ(size, n_i.size());
    for (std::size_t j = 0; j < size; ++j) {
      using ::fst::Plus;
      m_i[j] = Plus(m_i[j], n_i[j]);
    }
  }
  return true;
}

template <class W>
bool MatrixSemiring<W>::Times(Matrix *p, const Matrix &m, const Matrix &n) {
  std::size_t size = m.size();
  if (n.size() != size || p->size() != size) {
    return false;
  }
  for (std::size_t i = 0; i < size; ++i) {
    auto &p_i = (*p)[i];
    const auto &m_i = m[i];
    DCHECK_EQ(size, m_i.size());
    for (std::size_t j = 0; j < size; ++j) {
      W &p_ij = p_i[j];
      for (std::size_t k = 0; k < size; ++k) {
        DCHECK_EQ(size, n[k].size());
        using ::fst::Plus;
        using ::fst::Times;
        p_ij = Plus(p_ij, Times(m_i[k], n[k][j]));
      }
    }
  }
  return true;
}

// The Newton-Gauss-Jordan-Kleene-Roy-McNaughton+Yamada-Warshall-Floyd-Conway-
// Aho+Hopcroft+Ullman-Lehmann-Tarjan-Fletcher generalized all-pairs algebraic
// path a/k/a semiring matrix asteration algorithm. This version computes
// Star(m) in-place (see Jansche 2003, p. 176).
template <class W>
void MatrixSemiring<W>::Star(Matrix *m) {
  using ::fst::Plus;
  using ::fst::Times;
  using ::fst::Star;
  const std::size_t size = m->size();
  for (std::size_t k = 0; k < size; ++k) {
    auto &m_k = (*m)[k];
    DCHECK_EQ(size, m_k.size());
    W b = Star(m_k[k]);
    for (std::size_t i = 0; i < size; ++i) {
      if (i == k) {  // Postpone this case to make in-place updates correct.
        continue;
      }
      auto &m_i = (*m)[i];
      if (m_i[k] != W::Zero()) {
        W ab = Times(m_i[k], b);
        for (std::size_t j = 0; j < size; ++j) {
          m_i[j] = Plus(m_i[j], Times(ab, m_k[j]));
        }
      }
    }
    // Finish the case i == k that was skipped above:
    if (m_k[k] != W::Zero()) {
      W c = Plus(W::One(), Times(m_k[k], b));
      for (std::size_t j = 0; j < size; ++j) {
        m_k[j] = Times(c, m_k[j]);
      }
    }
    m_k[k] = Plus(m_k[k], W::One());
  }
}

// Returns an adjaceny matrix representation of the weighted graph corresponding
// to the given FST and arc filter. If the FST has n states, the adjacency
// matrix has dimension (n+1) x (n+1), where the last column corresponds to the
// final weights (and the last row is Zero). This is as if a super-final state
// (with state number n) had been added to the FST and the final weight of each
// ordinary state s (with 0 <= s < n) expressed instead as an arc from s to the
// super-final state n with the corresponding final weight. Thus the adjacency
// matrix contains complete information about all weights in the FST.
template <class F, class ArcFilter = AnyArcFilter<typename F::Arc>>
typename MatrixSemiring<typename F::Weight>::Matrix AdjacencyMatrix(
    const F &fst, ArcFilter arc_filter = ArcFilter()) {
  typedef typename F::Arc Arc;
  typedef typename F::StateId StateId;
  typedef typename F::Weight Weight;
  const StateId num_states = CountStates(fst);
  auto matrix = MatrixSemiring<Weight>::Zero(num_states + 1);
  if (num_states == 0) {
    return matrix;
  }
  for (StateId source = 0; source < num_states; ++source) {
    auto &source_to = matrix[source];
    for (ArcIterator<F> iter(fst, source); !iter.Done(); iter.Next()) {
      const Arc &arc = iter.Value();
      if (!arc_filter(arc)) {
        continue;
      }
      StateId target = arc.nextstate;
      DCHECK_GE(target, 0);
      DCHECK_LT(target, num_states);
      source_to[target] = Plus(source_to[target], arc.weight);
    }
    source_to[num_states] = fst.Final(source);
  }
  return matrix;
}

// Computes the all-pairs algebraic path matrix for the given FST. The name
// AllPairsDistance was chosen for similarity with the OpenFst library, which
// has a corresponding single-source ShortestDistance function. Refer to the
// returned distance matrix as D; then D[i][j] contains the semiring sum over
// all paths (possibly infinitely many if the FST is cyclic) from state i to
// state j. Note that D contains distances to an implicit super-final state with
// state number n = fst.NumStates() (see the comments for AdjacencyMatrix above
// for further details). In particular D[fst.Start()][fst.NumStates()] holds the
// semiring sum over all accepting paths through the FST.
template <class F, class ArcFilter = AnyArcFilter<typename F::Arc>>
typename MatrixSemiring<typename F::Weight>::Matrix AllPairsDistance(
    const F &fst) {
  auto matrix = AdjacencyMatrix(fst, ArcFilter());
  MatrixSemiring<typename F::Weight>::Star(&matrix);
  return matrix;
}

// Computes the total distance of the given FST (graph), which is simply the
// distance from its start state to its implicit super-final state.
template <class F, class ArcFilter = AnyArcFilter<typename F::Arc>>
typename F::Weight TotalDistance(const F &fst) {
  auto weight = F::Weight::Zero();
  if (fst::kNoStateId != fst.Start()) {
    auto matrix = AllPairsDistance(fst);
    weight = matrix[fst.Start()].back();
  }
  return weight;
}

}  // namespace fst

#endif  // FESTUS_MATRIX_H__

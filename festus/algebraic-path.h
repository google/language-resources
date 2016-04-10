// festus/algebraic-path.h
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
// Algebraic path computation.
//
// The algebraic path problem asks for the evaluation of the sum of the weights
// of all paths through a weighted graph (or FST in our case), where the weight
// of a path is the semiring product of the weights of its arcs, and where
// paths that share endpoints can be summed up using semiring addition.

#ifndef FESTUS_ALGEBRAIC_PATH_H__
#define FESTUS_ALGEBRAIC_PATH_H__

#include <cstddef>
#include <type_traits>
#include <vector>

#include <fst/fst.h>

namespace festus {
namespace internal {

template <typename T>
using Matrix = std::vector<std::vector<T>>;

template <typename T>
inline Matrix<T> MakeSquareMatrix(std::size_t count, const T &val) {
  return Matrix<T>(count, std::vector<T>(count, val));
}

// Returns an adjaceny matrix representation of the weighted graph underlying
// the given FST. If the FST has n states, the adjacency matrix has dimension
// (n+1) x (n+1), where the last column corresponds to the final weights (and
// the last row is Zero). This is as if a super-final state (with state number
// n) had been added to the FST and the final weight of each ordinary state s
// (with 0 <= s < n) expressed instead as an arc from s to the super-final
// state n with the corresponding final weight. Thus the adjacency matrix
// contains complete information about all weights in the FST.
template <class F, class S>
Matrix<typename S::ValueType> AdjacencyMatrix(const F &fst, S *sr) {
  typedef typename F::Arc Arc;
  typedef typename F::StateId StateId;
  const StateId num_states = CountStates(fst);
  auto matrix = MakeSquareMatrix(num_states + 1, sr->Zero());
  if (num_states == 0) {
    return matrix;
  }
  for (StateId source = 0; source < num_states; ++source) {
    auto &source_to = matrix[source];
    for (fst::ArcIterator<F> iter(fst, source); !iter.Done(); iter.Next()) {
      const Arc &arc = iter.Value();
      StateId target = arc.nextstate;
      CHECK_GE(target, 0);
      CHECK_LT(target, num_states);
      source_to[target] = sr->OpPlus(source_to[target], arc.weight.Value());
    }
    source_to[num_states] = fst.Final(source).Value();
  }
  return matrix;
}

template <class M, class S>
void MatrixKleenePlus(M *matrix, S *sr) {
  M &m = *matrix;
  const std::size_t size = m.size();
  for (std::size_t k = 0; k < size; ++k) {
    auto &m_k = m[k];
    CHECK_EQ(size, m_k.size());
    auto b = sr->OpStar(m_k[k]);
    // Don't bother to check if sr->NotZero(b), as that can only be false in
    // the case of the zero/trivial semiring.
    for (std::size_t i = 0; i < size; ++i) {
      if (i == k) {  // Postpone this case to make in-place updates correct.
        continue;
      }
      auto &m_i = m[i];
      CHECK_EQ(size, m_i.size());
      if (sr->NotZero(m_i[k])) {
        auto ab = sr->OpTimes(m_i[k], b);
        for (std::size_t j = 0; j < size; ++j) {
          m_i[j] = sr->OpPlus(m_i[j], sr->OpTimes(ab, m_k[j]));
        }
      }
    }
    // Finish the case i == k that was skipped above:
    if (sr->NotZero(m_k[k])) {
      auto ab = sr->OpTimes(m_k[k], b);
      for (std::size_t j = 0; j < size; ++j) {
        m_k[j] = sr->OpPlus(m_k[j], sr->OpTimes(ab, m_k[j]));
      }
    }
  }
}

// Adapter that presents an OpenFst weight class as a semiring class.
// Has just enough definitions to instantiate MatrixKleenePlus.
template <class W>
struct SemiringForValueWeight {
  typedef W Weight;
  typedef typename std::decay<decltype(W::Zero().Value())>::type ValueType;

  static constexpr bool Member(ValueType x) { return Weight(x).Member(); }

  static constexpr bool NotZero(ValueType x) {
    return Weight(x) != Weight::Zero();
  }

  static constexpr ValueType Zero() { return Weight::Zero().Value(); }

  static constexpr ValueType OpPlus(ValueType x, ValueType y) {
    return Plus(Weight(x), Weight(y)).Value();
  }

  static constexpr ValueType OpTimes(ValueType x, ValueType y) {
    return Times(Weight(x), Weight(y)).Value();
  }

  static constexpr ValueType OpStar(ValueType x) {
    return Star(Weight(x)).Value();
  }
};

// TMP helper template and specializations for retrieving a semiring
// instance for the given weight W.
//
// If class W has a static member function W::Semiring() (which is the
// case for W = ValueWeightSingleton<...>), then
// SemiringFor<W>::Instance() dispatches to W::Semiring().
//
// Otherwise, if class W contains the typename SemiringType (which is
// the case for W = ValueWeightStatic<...>), then
// SemiringFor<W>::Instance() returns an object of type W::SemiringType.
//
// In all other cases SemiringFor<W>::Instance() returns an object of
// type SemiringForValueWeight<W>.
//
// Intended usage:
//
//   const auto &semiring = SemiringFor<W>::Instance();
//
template <class W, class HasSemiringType = void, class HasInstanceFunc = void>
struct SemiringFor {
  typedef SemiringForValueWeight<W> Type;
  static constexpr Type Instance() { return Type(); }
  static constexpr int IsSpecialized() { return 0; }
};

template <typename T> struct Void { typedef void type; };

template <class W, class HasInstanceFunc>
struct SemiringFor<W,
                   typename Void<typename W::SemiringType>::type,
                   HasInstanceFunc> {
  typedef typename W::SemiringType Type;
  static Type Instance() {
    typename W::SemiringType result;
    return result;
  }
  static constexpr int IsSpecialized() { return 1; }
};

template <class W>
struct SemiringFor<W,
                   typename Void<typename W::SemiringType>::type,
                   typename Void<decltype(W::Semiring())>::type> {
  typedef typename W::SemiringType Type;
  static constexpr const Type &Instance() { return W::Semiring(); }
  static constexpr int IsSpecialized() { return 2; }
};

}  // namespace internal

// Returns the algebraic sum total value (in the given semiring) of all paths.
//
// TODO: This implementation does too much. It essentially solves the all-pairs
// problem when a solution to the single-source problem is sufficient. Replace
// it with a single-source algorithm based on SCC partitioning.
template <class F, class S>
typename S::ValueType SumTotalValue(const F &fst, S *semiring) {
  const auto start = fst.Start();
  if (start < 0) {
    return semiring->Zero();
  }
  auto matrix = internal::AdjacencyMatrix(fst, semiring);
  for (const auto &row : matrix) {
    for (auto value : row) {
      if (!semiring->Member(value)) {
        LOG(ERROR) << "Adjacency matrix contains ill-formed value";
        return value;
      }
    }
  }
  internal::MatrixKleenePlus(&matrix, semiring);
  // If we were interested in the Star() of the matrix, we would need to add
  // semiring->One() to the diagonal. However, we only need to access one value
  // in the matrix, namely matrix[start][superfinal]. By construction, this
  // cannot be on the diagonal, so it is unaffected by adding One().
  CHECK_LT(start, matrix.size() - 1);
  return matrix[start].back();
}

template <class F>
typename F::Weight SumTotalWeight(const F &fst) {
  typedef internal::SemiringFor<typename F::Weight> SemiringForWeight;
  VLOG(1) << "festus::SumTotalWeight() uses "
          << (SemiringForWeight::IsSpecialized()
              ? "semiring from weight facade"
              : "SemiringForValueWeight adapter");
  const auto &semiring = SemiringForWeight::Instance();
  return SumTotalValue(fst, &semiring);
}

}  // namespace festus

#endif  // FESTUS_ALGEBRAIC_PATH_H__

// festus/fst-util.h
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
// Utility functions for working with FSTs.

#ifndef FESTUS_FST_UTIL_H__
#define FESTUS_FST_UTIL_H__

#include <cstddef>
#include <limits>
#include <vector>

#include <fst/compat.h>
#include <fst/connect.h>
#include <fst/expanded-fst.h>
#include <fst/fst.h>
#include <fst/shortest-path.h>
#include <fst/topsort.h>
#include <fst/vector-fst.h>

#include "festus/label-maker.h"

namespace festus {

constexpr uint64 kConnectedAndTopSorted =
    fst::kAccessible | fst::kCoAccessible | fst::kTopSorted;

template <class F>
std::size_t CountPathsTopSorted(const F &fst) {
  typedef typename F::Arc::StateId StateId;
  typedef typename F::Arc::Weight Weight;
  DCHECK_EQ(kConnectedAndTopSorted,
            fst.Properties(kConnectedAndTopSorted, false));
  const StateId num_states = fst::CountStates(fst);
  if (num_states <= 0) {
    return 0;
  }
  DCHECK_EQ(0, fst.Start());
  std::size_t total_paths = 0;
  std::vector<std::size_t> paths(num_states, 0);
  paths[0] = 1;
  for (StateId s = 0; s < num_states; ++s) {
    std::size_t paths_to_s = paths[s];
    if (fst.Final(s) != Weight::Zero()) {
      // There must be a nonzero number of paths reaching s. Otherwise s would
      // not be accessible, which is impossible under the precondition that the
      // FST must be connected.
      DCHECK_GT(paths_to_s, 0);
      total_paths += paths_to_s;
    }
    for (fst::ArcIterator<F> aiter(fst, s); !aiter.Done(); aiter.Next()) {
      const auto &arc = aiter.Value();
      StateId t = arc.nextstate;
      DCHECK_GT(t, s);
      DCHECK_LT(t, num_states);
      paths[t] += paths_to_s;
    }
  }
  return total_paths;
}

template <class F>
double CountPaths(const F &fst) {
  if (fst::kNoStateId == fst.Start()) {
    return 0;
  }
  if (fst.Properties(kConnectedAndTopSorted, false) == kConnectedAndTopSorted) {
    return CountPathsTopSorted(fst);
  }
  // Function is templated on FST type F to take advantage of overloading of
  // VectorFst ctor to handle copying of a VectorFst more efficiently.
  fst::VectorFst<typename F::Arc> vector_fst(fst);
  fst::Connect(&vector_fst);
  bool acyclic = fst::TopSort(&vector_fst);
  if (acyclic) {
    return CountPathsTopSorted(vector_fst);
  } else {
    return std::numeric_limits<double>::infinity();
  }
}

template <class F>
string OneString(const F &fst, typename F::StateId state,
                 const LabelMaker &label_maker) {
  typedef typename F::Arc Arc;
  typedef typename F::Weight Weight;
  CHECK_NE(state, fst::kNoStateId);
  LabelMaker::Labels labels;
  while (fst.Final(state) == Weight::Zero()) {
    fst::ArcIterator<F> iter(fst, state);
    CHECK(!iter.Done());
    const Arc &arc = iter.Value();
    if (arc.ilabel != 0) {
      labels.push_back(arc.ilabel);
    }
    CHECK_NE(arc.nextstate, fst::kNoStateId);
    state = arc.nextstate;
    DCHECK((iter.Next(), iter.Done()));
  }
  DCHECK(fst::ArcIterator<F>(fst, state).Done());
  string s;
  CHECK(label_maker.LabelsToString(labels, &s));
  return s;
}

template <class F>
inline string OneString(const F &fst, const LabelMaker &label_maker) {
  return OneString(fst, fst.Start(), label_maker);
}

template <class F>
void NStrings(const F &fst, size_t n, const LabelMaker &label_maker,
              std::vector<string> *strings) {
  typedef typename F::Arc Arc;
  typedef typename F::StateId StateId;
  const StateId start = fst.Start();
  if (start == fst::kNoStateId) {
    return;
  }
  fst::VectorFst<Arc> paths;
  fst::ShortestPath(fst, &paths, n);
  for (fst::ArcIterator<F> iter(paths, start); !iter.Done(); iter.Next()) {
    const Arc &arc = iter.Value();
    CHECK_EQ(arc.ilabel, 0);
    CHECK_EQ(arc.olabel, 0);
    strings->emplace_back(OneString(paths, arc.nextstate, label_maker));
  }
}

}  // namespace festus

#endif  // FESTUS_FST_UTIL_H__

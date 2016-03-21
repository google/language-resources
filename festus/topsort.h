// festus/topsort.h
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
// Generalized topological sorting of FSTs in the presence of self-loops.
//
// A graph is (strictly) topologically ordered [resp. orderable] if its
// adjacency matrix is [resp. can be rearranged to be] (strictly) upper
// triangular. Nonzero diagonal elements of the adjacency matrix correspond to
// self-loops (cycles of length 1). To produce a generalized topological
// ordering, use the usual Depth-First Search algorithm, but ignore back arcs
// that are loops.

#ifndef FESTUS_TOPSORT_H__
#define FESTUS_TOPSORT_H__

#include <vector>

#include <fst/compat.h>
#include <fst/dfs-visit.h>
#include <fst/fst.h>
#include <fst/statesort.h>

namespace festus {

// DFS visitor class that returns a toplogical state ordering, even in the
// presence of self-loops. Based on fst::TopOrderVisitor.
template <class A>
class LoopyTopOrderVisitor {
 public:
  typedef A Arc;
  typedef typename A::StateId StateId;

  void InitVisit(const fst::Fst<A> &fst) {
    start_ = fst.Start();
    no_loops_ = true;
    no_cycles_ = true;
    no_initial_cycles_ = true;
    finish_.clear();
  }

  bool InitState(StateId, StateId) { return true; }

  bool TreeArc(StateId, const A &) { return true; }

  bool BackArc(StateId s, const A &arc) {
    if (arc.nextstate == s) {
      no_loops_ = false;
    } else {
      no_cycles_ = false;
    }
    if (arc.nextstate == start_) {
      no_initial_cycles_ = false;
    }
    return true;
  }

  bool ForwardOrCrossArc(StateId, const A &) { return true; }

  void FinishState(StateId s, StateId, const A *) { finish_.emplace_back(s); }

  void FinishVisit() {}

  bool Sortable() const { return no_cycles_; }

  bool Acyclic() const { return no_loops_ && no_cycles_; }

  bool InitialAcyclic() const { return no_initial_cycles_; }

  // Returns the reordering vector for reverse finishing-time order, in the
  // format required by fst::StateSort.
  std::vector<StateId> Order() const {
    std::vector<StateId> order(finish_.size(), fst::kNoStateId);
    for (StateId s = 0; s < finish_.size(); ++s) {
      order[finish_[finish_.size() - s - 1]] = s;
    }
    return order;
  }

 private:
  StateId start_;
  bool no_loops_;
  bool no_cycles_;
  bool no_initial_cycles_;
  std::vector<StateId> finish_;
};

// Sorts the states of an FST in generalized topological order, if possible.
// A generalized topologically order exists if every cycle in the input FST is
// a self-loop.
//
// Returns true iff the output FST is in generalized toplogical
// order. Always sets the property bits for cycles, initial cycles, and
// strict topological sortedness.
//
// NB: If the input FST has loops but no other cycles, this function will
// return true and set the property bits fst::kCyclic and fst::kNotTopSorted,
// since the definition of fst::kTopSorted implies fst::kAcyclic.
template <class Arc>
bool LoopyTopSort(fst::MutableFst<Arc> *fst) {
  if (fst::kNoStateId == fst->Start()) {
    fst->SetProperties(fst::kNullProperties, fst::kNullProperties);
    return true;
  }

  LoopyTopOrderVisitor<Arc> visitor;
  fst::DfsVisit(*fst, &visitor);

  if (visitor.Sortable()) {
    fst::StateSort(fst, visitor.Order());
  }

  if (visitor.Acyclic()) {
    fst->SetProperties(fst::kAcyclic | fst::kInitialAcyclic | fst::kTopSorted,
                       fst::kAcyclic | fst::kInitialAcyclic | fst::kTopSorted);
  } else {
    fst->SetProperties(fst::kCyclic | fst::kNotTopSorted,
                       fst::kCyclic | fst::kNotTopSorted);
    if (visitor.InitialAcyclic()) {
      fst->SetProperties(fst::kInitialAcyclic, fst::kInitialAcyclic);
    } else {
      fst->SetProperties(fst::kInitialCyclic, fst::kInitialCyclic);
    }
  }

  return visitor.Sortable();
}

}  // namespace festus

#endif  // FESTUS_TOPSORT_H__

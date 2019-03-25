// Copyright 2019 Google LLC. All Rights Reserved.
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

// \file
// Library for conflating epsilon-cycles in an FST.

#ifndef FESTUS_RMEPSCYCLE_H__
#define FESTUS_RMEPSCYCLE_H__

#include <cstddef>
#include <vector>

#include <fst/arcfilter.h>
#include <fst/compat.h>
#include <fst/connect.h>
#include <fst/dfs-visit.h>
#include <fst/fst.h>
#include <fst/mutable-fst.h>
#include <fst/shortest-distance.h>

namespace festus {

// Arc filter that restricts arcs to those within the same strongly connected
// component (SCC) of the epsilon-graph. Similar to fst::EpsilonArcFilter
// declared in <fst/arcfilter.h>, with the additional restriction that arcs are
// retained iff they lead to a state belonging to a given SCC. The vector `scc`
// passed as a constructor argument specifies a state-to-component mapping and
// can be computed using fst::SccVisitor declared in <fst/connect.h>.
template <class Arc, class Component = typename Arc::StateId>
class EpsilonSccArcFilter {
 public:
  explicit EpsilonSccArcFilter(const std::vector<Component> *scc) : scc_(scc) {}

  void SetComponent(Component component) { component_ = component; }

  bool operator()(const Arc &arc) const {
    return arc.ilabel == 0 && arc.olabel == 0 &&
           scc_->at(arc.nextstate) == component_;
  }

 private:
  const std::vector<Component> *const scc_;
  Component component_;
};

// Ensures that the epsilon-graph of the FST is acyclic. Identifies strongly
// connected components (SCCs) within the epsilon-graph and turns each SCC of N
// states into a complete bipartite graph with 2*N states.
template <class Arc>
void RmEpsilonCycle(fst::MutableFst<Arc> *fst,
                    float delta = fst::kShortestDelta) {
  using Label = typename Arc::StateId;
  using StateId = typename Arc::StateId;
  using Weight = typename Arc::Weight;
  if (fst->Start() < 0) {
    fst->DeleteStates();
    return;
  }
  // Do a DFS pass to find strongly connected components (SCCs).
  std::vector<StateId> scc;
  uint64 properties = 0;
  fst::SccVisitor<Arc> scc_visitor(&scc, nullptr, nullptr, &properties);
  fst::EpsilonArcFilter<Arc> eps_filter;
  fst::DfsVisit(*fst, &scc_visitor, eps_filter);
  switch (properties & (fst::kCyclic | fst::kAcyclic)) {
    case fst::kAcyclic:
      VLOG(1) << "Epsilon-graph is acyclic";
      return;
    case fst::kCyclic:
      VLOG(1) << "Epsilon-graph is cyclic";
      break;
    default:
      VLOG(1) << "Epsilon-graph cyclicity cannot be determined";
      break;
  }
  // Populate auxiliary data structures for navigating SCCs.
  std::vector<StateId> first(scc.size(), fst::kNoStateId);
  std::vector<StateId> next(scc.size(), fst::kNoStateId);
  for (std::size_t state = 0; state < scc.size(); ++state) {
    const StateId component = scc[state];
    if (first[component] != fst::kNoStateId) next[state] = first[component];
    first[component] = state;
  }
  // The smallest SCC size computed by SccVisitor is 1. Further differentiate
  // those into size 0 (no epsilon self-loops) vs. proper size 1 (at least one
  // epsilon self-loop).
  for (StateId &state : first) {
    if (fst::kNoStateId == state || next[state] != fst::kNoStateId) {
      // SCC size is not equal to 1.
      continue;
    }
    bool found_loop = false;
    // If there are no arcs with epsilons, there cannot be any epsilon-loops.
    if (fst->NumInputEpsilons(state) != 0) {
      // If there are epsilon-arcs, look for self-loops.
      using AIter = fst::ArcIterator<fst::MutableFst<Arc>>;
      for (AIter aiter(*fst, state); !aiter.Done(); aiter.Next()) {
        const Arc &arc = aiter.Value();
        if (eps_filter(arc)) {
          found_loop |= (arc.nextstate == state);
        }
      }
    }
    if (!found_loop) {
      state = fst::kNoStateId;
    }
  }
  // Split each state in an SCC of size >= 1 into two by adding a new source
  // state for a given state in an SCC. Add epsilon transition from the new
  // source state to all other states within the same SCC. The weight of the
  // newly added transitions is the algebraic distance in the epsilon-graph.
  using Queue = fst::FifoQueue<StateId>;
  using ArcFilter = EpsilonSccArcFilter<Arc>;
  using SDOptions = fst::ShortestDistanceOptions<Arc, Queue, ArcFilter>;
  constexpr Label kEpsilon = 0;
  Queue queue;
  ArcFilter eps_scc_filter(&scc);
  std::vector<Weight> distance;
  std::vector<StateId> split_state(scc.size(), fst::kNoStateId);
  for (std::size_t state = 0; state < scc.size(); ++state) {
    const StateId component = scc[state];
    if (fst::kNoStateId == first[component]) continue;
    eps_scc_filter.SetComponent(component);
    const SDOptions sd_options(&queue, eps_scc_filter, state, delta);
    fst::ShortestDistance(*fst, &distance, sd_options);
    const StateId s = fst->AddState();
    for (StateId t = first[component]; t != fst::kNoStateId; t = next[t]) {
      fst->AddArc(s, Arc(kEpsilon, kEpsilon, distance[t], t));
    }
    split_state[state] = s;
    VLOG(1) << "Splitting state into pair (" << s << ", " << state << ")";
  }
  // Redirect arcs that reach states within an epsilon-graph SCC. If an
  // epsilon-arc reaches another state within the same SCC, that arc is slated
  // for deletion by retargeting it at a newly added sink state. Otherwise, if
  // the target state of the arc has been split in the preceding step, redirect
  // it to reach the newly added split state.
  const StateId sink = fst->AddState();
  for (std::size_t state = 0; state < scc.size(); ++state) {
    eps_scc_filter.SetComponent(scc[state]);
    using MutableAIter = fst::MutableArcIterator<fst::MutableFst<Arc>>;
    for (MutableAIter aiter(fst, state); !aiter.Done(); aiter.Next()) {
      const Arc &arc = aiter.Value();
      StateId new_nextstate = fst::kNoStateId;
      if (eps_scc_filter(arc)) {
        new_nextstate = sink;
      } else {
        new_nextstate = split_state[arc.nextstate];
      }
      if (new_nextstate != fst::kNoStateId) {
        Arc new_arc = arc;
        new_arc.nextstate = new_nextstate;
        aiter.SetValue(new_arc);
      }
    }
  }
  // If the start state has been split, adjust accordingly.
  const StateId split_start = split_state[fst->Start()];
  if (split_start != fst::kNoStateId) {
    fst->SetStart(split_start);
  }
  fst::Connect(fst);
}

}  // namespace festus

#endif  // FESTUS_RMEPSCYCLE_H__

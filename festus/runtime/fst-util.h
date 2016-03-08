// festus/runtime/g2p.h
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
// Various operations on FSTs.

#ifndef FESTUS_RUNTIME_FST_UTIL_H__
#define FESTUS_RUNTIME_FST_UTIL_H__

#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <set>
#include <utility>
#include <vector>

#include <fst/compat.h>
#include <fst/fstlib.h>

namespace festus {

string PropertiesToString(uint64 props) {
  string str;
  uint64 p = 1;
  for (int i = 0; i < 64; ++i, p <<= 1) {
    if (p & props) {
      str.append(str.empty() ? "" : ", ");
      str.append(fst::PropertyNames[i]);
    }
  }
  return str;
}

template <class Arc>
bool HasStoredProperties(const fst::Fst<Arc> &fst, uint64 mask) {
  uint64 stored = fst.Properties(fst::kFstProperties, false);
#ifndef NDEBUG
  uint64 known;
  CHECK(fst::CompatProperties(
      stored, fst::ComputeProperties(fst, mask, &known, false)));
#endif  // NDEBUG
  return (stored & mask) == mask;
}

template <class Arc>
void ExpectProperties(const fst::Fst<Arc> &fst, uint64 mask) {
  uint64 props = fst.Properties(mask, false);
  if (props != mask) {
    LOG(WARNING) << "Unexpected stored properties:";
    LOG(WARNING) << "  Observed: " << PropertiesToString(props);
    LOG(WARNING) << "  Expected: " << PropertiesToString(mask);
    props = fst.Properties(mask, true);
    if (props != mask) {
      LOG(ERROR) << "Unexpected computed properties:";
      LOG(ERROR) << "  Observed: " << PropertiesToString(props);
      LOG(ERROR) << "  Expected: " << PropertiesToString(mask);
    }
  }
}

template <class Arc>
string DebugProperties(
    const fst::Fst<Arc> &fst,
    uint64 mask = fst::kFstProperties) {
  uint64 stored = fst.Properties(mask, false);
  uint64 known, computed = fst::ComputeProperties(fst, mask, &known, false);
  string str;
  uint64 p = 1;
  for (int i = 0; i < 64; ++i, p <<= 1) {
    if (p & stored) {
      str.append(str.empty() ? "" : ", ");
      str.append(fst::PropertyNames[i]);
      if (!(p & computed)) {
        str.append(" [WRONG]");
      }
    } else if (p & computed) {
      str.append(str.empty() ? "(" : ", (");
      str.append(fst::PropertyNames[i]);
      str.append(")");
    }
  }
  return str;
}

#define VLOG_PROPERTIES(n, fst)                                     \
  do {                                                              \
    if (FLAGS_v >= (n)) {                                           \
      LOG(INFO) << #fst << " properties: " << DebugProperties(fst); \
    }                                                               \
  } while (false)

template <class Arc>
void ConnectAndComputeProperties(fst::MutableFst<Arc> *fst) {
  typedef typename Arc::StateId StateId;
  CHECK(HasStoredProperties(*fst, fst::kAccessible));
  std::vector<bool> access, coaccess;
  access.reserve(fst->NumStates());
  coaccess.reserve(fst->NumStates());
  uint64 props = 0;
  fst::SccVisitor<Arc> scc_visitor(nullptr, &access, &coaccess, &props);
  fst::DfsVisit(*fst, &scc_visitor);
  VLOG(3) << "DFS properties: " << PropertiesToString(props);
  if (props & fst::kNotCoAccessible) {
    std::vector<StateId> dstates;
    for (StateId s = 0; s < coaccess.size(); ++s) {
      CHECK_LT(s, access.size());
      CHECK(access[s]);
      if (!coaccess[s]) {
        dstates.push_back(s);
      }
    }
    fst->DeleteStates(dstates);
  }
  static constexpr uint64 kAccessibilityMask =
      fst::kAccessible | fst::kNotAccessible |
      fst::kCoAccessible | fst::kNotCoAccessible;
  fst->SetProperties(fst::kAccessible | fst::kCoAccessible,
                     kAccessibilityMask);
  static constexpr uint64 kCyclicityMask =
      fst::kCyclic | fst::kAcyclic |
      fst::kInitialCyclic | fst::kInitialAcyclic;
  if (props & fst::kAcyclic) {
    // The FST was found to be acyclic during DST. After removing unconnected
    // states and their incident arcs, the updated FST must still be acyclic.
    fst->SetProperties(fst::kAcyclic | fst::kInitialAcyclic, kCyclicityMask);
  } else {
    // The FST was found to be cyclic during DFS. After removing unconnected
    // states and their incident arcs, the FST may or may not be cyclic.
    fst->Properties(fst::kCyclic, true);
  }
  if (fst->Properties(fst::kAcyclic, false)) {
    fst->Properties(fst::kTopSorted, true);
  } else {
    fst->SetProperties(fst::kNotTopSorted,
                       fst::kTopSorted | fst::kNotTopSorted);
  }
}

// Compose, project, connect, and remove epsilons.
template <class Arc>
void ComposeProjectRmEpsilon(
    const fst::Fst<Arc> &ifst1,
    const fst::Fst<Arc> &ifst2,
    fst::ProjectType project_type,
    fst::MutableFst<Arc> *ofst,
    float delta = fst::kDelta,
    bool use_trivial_filter = false) {  // ignored (requires OpenFst 1.5.1)
  fst::CacheOptions nopts;
  nopts.gc_limit = 0;  // Cache only the last state for fastest copy.
  fst::ComposeFst<Arc> composed(ifst1, ifst2, nopts);
  *ofst = fst::ProjectFst<Arc>(composed, project_type);
  VLOG_PROPERTIES(3, *ofst);
  ConnectAndComputeProperties(ofst);
  VLOG_PROPERTIES(3, *ofst);
  if (ofst->Properties(fst::kEpsilons, false)) {
    fst::RmEpsilon(ofst, false, Arc::Weight::Zero(), fst::kNoStateId, delta);
    VLOG_PROPERTIES(3, *ofst);
  } else {
    VLOG(2) << "Skipping epsilon removal: acceptor is already epsilon-free";
  }
}

template <class A>
using DefaultPhiMatcher = fst::PhiMatcher<fst::Matcher<fst::Fst<A>>>;

template <class A>
fst::ComposeFstOptions<A, DefaultPhiMatcher<A>> PhiComposeOptions(
    const fst::Fst<A> &fst1,
    const fst::Fst<A> &fst2,
    typename A::Label phi_label,
    const fst::CacheOptions &cache_options = fst::CacheOptions()) {
  return fst::ComposeFstOptions<A, DefaultPhiMatcher<A>>(
      cache_options,
      new DefaultPhiMatcher<A>(fst1, fst::MATCH_NONE),
      new DefaultPhiMatcher<A>(fst2, fst::MATCH_INPUT, phi_label,
                               true, fst::MATCHER_REWRITE_NEVER));
}

// Lazy composition with a phi-FST on the right.
//
// Composes an arbitrary machine fst1 with an fst2 that contains failure
// or backoff arcs (e.g. a backoff language model). During composition
// arcs of fst2 with input label phi_label (assuming phi_label != kNoLabel)
// are interpreted as failure/backoff. See the OpenFst Advanced Usage guide
// for additional details.
template <class A>
fst::ComposeFst<A> PhiComposeFst(
    const fst::Fst<A> &fst1,  // arbitrary FST
    const fst::Fst<A> &fst2,  // FST with phi labels, e.g. backoff model
    typename A::Label phi_label,
    const fst::CacheOptions &cache_options = fst::CacheOptions()) {
  return fst::ComposeFst<A>(
      fst1, fst2,
      PhiComposeOptions(fst1, fst2, phi_label, cache_options));
}

// Eager composition with a phi-FST on the right.
template <class Arc>
void PhiCompose(
    const fst::Fst<Arc> &ifst1,  // arbitrary FST
    const fst::Fst<Arc> &ifst2,  // FST with phi labels, e.g. backoff model
    typename Arc::Label phi_label,
    fst::MutableFst<Arc> *ofst,
    bool connect = true) {
  fst::CacheOptions nopts;
  nopts.gc_limit = 0;  // Cache only the last state for fastest copy.
  *ofst = PhiComposeFst<Arc>(ifst1, ifst2, phi_label, nopts);
  if (connect) {
    ConnectAndComputeProperties(ofst);
  }
}

// Appends the labels (either input or output labels) to an output iterator.
// Can be used with a std::insert_iterator on a set to compute the label set.
template <class F, class OutputIterator>
void GetLabels(
    const F &fst,
    bool use_ilabels,
    OutputIterator oiter) {
  for (fst::StateIterator<F> siter(fst); !siter.Done(); siter.Next()) {
    auto s = siter.Value();
    for (fst::ArcIterator<F> aiter(fst, s); !aiter.Done(); aiter.Next()) {
      const auto &arc = aiter.Value();
      *oiter++ = use_ilabels ? arc.ilabel : arc.olabel;
    }
  }
}

// Constructs a lazy phi-removed FST.
template <class Arc>
fst::ComposeFst<Arc> RmPhiFst(
    const fst::Fst<Arc> &fst,
    typename Arc::Label phi_label) {
  typedef typename Arc::Label Label;
  typedef typename Arc::StateId StateId;
  typedef typename Arc::Weight Weight;

  // Find the alphabet of input labels of the FST.
  std::set<Label> alphabet;
  GetLabels(fst, true, std::inserter(alphabet, alphabet.begin()));

  // Make an acceptor for the language freely generated by the alphabet
  // (without the special epsilon or phi labels).
  fst::VectorFst<Arc> fsa;
  const StateId s = fsa.AddState();
  fsa.SetStart(s);
  fsa.SetFinal(s, Weight::One());
  fsa.ReserveArcs(s, alphabet.size());
  for (Label label : alphabet) {
    if (label == 0 || label == phi_label || label == fst::kNoLabel) {
      continue;
    }
    fsa.AddArc(s, Arc(label, label, Weight::One(), s));
  }
  fsa.SetOutputSymbols(fst.InputSymbols());
  fsa.Properties(fst::kFstProperties, true);
  VLOG_PROPERTIES(3, fsa);

  // Return the phi-composition of fsa and fst.
  return PhiComposeFst(fsa, fst, phi_label);
}

// Trivial conversion between FSTs with different weight (arc) types.
template <class A, class B>
inline void ConvertWeight(const fst::Fst<A> &ifst, fst::MutableFst<B> *ofst) {
  fst::ArcMap(ifst, ofst, fst::WeightConvertMapper<A, B>());
}

// Lazy determinization followed by eager trivial weight conversion.
template <class A, class B>
void DeterminizeConvertWeight(
    const fst::Fst<A> &ifst,
    fst::MutableFst<B> *ofst,
    float delta = fst::kDelta) {
  fst::DeterminizeFstOptions<A> nopts;
  nopts.delta = delta;
  nopts.gc_limit = 0;  // Cache only the last state for fastest copy.
  fst::DeterminizeFst<A> determinized(ifst, nopts);
  ConvertWeight(determinized, ofst);
}

// Converts the output FST of ShortestPath() into vector form.
template <class F>
void PathsToVector(
    const F &paths_fst,
    std::vector<std::pair<string, float>> *paths,
    typename F::Arc::Weight total_weight = F::Arc::Weight::One()) {
  typedef typename F::Arc Arc;
  typedef typename F::StateId StateId;
  typedef typename F::Weight Weight;
  paths->clear();
  const StateId start = paths_fst.Start();
  if (start == fst::kNoStateId) return;
  paths->reserve(paths_fst.NumArcs(start));
  const fst::SymbolTable *symbols = paths_fst.OutputSymbols();
  CHECK(symbols != nullptr);
  typedef fst::ArcIterator<fst::VectorFst<Arc>> MyArcIterator;
  for (MyArcIterator iter1(paths_fst, start); !iter1.Done(); iter1.Next()) {
    const auto &first_arc = iter1.Value();
    string str = first_arc.olabel != 0 ? symbols->Find(first_arc.olabel) : "";
    Weight weight = first_arc.weight;
    StateId state = first_arc.nextstate;
    CHECK_NE(state, fst::kNoStateId);
    while (paths_fst.Final(state) == Weight::Zero()) {
      fst::ArcIterator<fst::VectorFst<Arc>> iter(paths_fst, state);
      const Arc &arc = iter.Value();
      if (arc.olabel != 0) {
        if (!str.empty()) str.push_back(' ');
        str.append(symbols->Find(arc.olabel));
      }
      weight = fst::Times(weight, arc.weight);
      state = arc.nextstate;
      CHECK_NE(state, fst::kNoStateId);
      CHECK((iter.Next(), iter.Done()));
    }
    CHECK(fst::ArcIterator<fst::VectorFst<Arc>>(paths_fst, state).Done());
    weight = fst::Times(weight, paths_fst.Final(state));
    weight = fst::Divide(weight, total_weight);
    paths->emplace_back(std::move(str), std::exp(-weight.Value()));
  }
}

constexpr uint64 kConnected = fst::kAccessible | fst::kCoAccessible;
constexpr uint64 kConnectedAndTopSorted = kConnected | fst::kTopSorted;

// Counts the number of accepting paths in a connected and topologically
// sorted FST (graph). This is essentially the same as:
//
//   ShortestDistance(
//       ArcMapFst<...>(fst, RmWeightMapper<F::Weight, NaturalWeight>()))
//
// where NaturalWeight represents the weight semiring of natural numbers.
template <class F>
std::size_t CountPathsTopSorted(const F &fst) {
  typedef typename F::Arc::StateId StateId;
  typedef typename F::Arc::Weight Weight;
  CHECK_EQ(kConnectedAndTopSorted,
           fst.Properties(kConnectedAndTopSorted, false));
  const StateId num_states = fst::CountStates(fst);
  if (num_states <= 0) {
    return 0;
  }
  // Because the FST is connected, topologically sorted, and nonempty, its
  // start state must be state 0. Assume counterfactually that the start state
  // was some other state s > 0. Then state 0 must be reachable from s, since
  // the FST is connected. This means that state 0 must have an incoming arc
  // originating from some state t > 0 on the path from s to state 0. But this
  // cannot be the case because the FST is topologically sorted.
  CHECK_EQ(0, fst.Start());
  std::size_t total_paths = 0;
  std::vector<std::size_t> paths(num_states, 0);
  paths[0] = 1;
  for (StateId s = 0; s < num_states; ++s) {
    std::size_t paths_to_s = paths[s];
    if (fst.Final(s) != Weight::Zero()) {
      // There must be a nonzero number of paths reaching s. Otherwise s would
      // not be accessible, which is impossible under the precondition that the
      // FST must be connected.
      CHECK_GT(paths_to_s, 0);
      total_paths += paths_to_s;
    }
    for (fst::ArcIterator<F> aiter(fst, s); !aiter.Done(); aiter.Next()) {
      const auto &arc = aiter.Value();
      StateId t = arc.nextstate;
      CHECK_GT(t, s);
      CHECK_LT(t, num_states);
      paths[t] += paths_to_s;
    }
  }
  return total_paths;
}

// Counts the number of accepting paths in a mutable FST. If the FST argument
// is not already connected and topologically sorted, this will connect and
// topologically sort the machine, but leave it otherwise unchanged.
template <class F>
double CountPaths(F *fst) {
  auto properties = fst->Properties(kConnectedAndTopSorted, false);
  if (kConnectedAndTopSorted == properties) {
    VLOG(3) << "CountPaths(Fst *): already connected and top. sorted";
  }
  if ((properties & kConnected) != kConnected) {
    fst::Connect(fst);
  }
  bool topsorted = (properties & fst::kTopSorted) || fst::TopSort(fst);
  if (topsorted) {
    return CountPathsTopSorted(*fst);
  } else {
    return std::numeric_limits<double>::infinity();
  }
}

// Counts the number of accepting paths in a const FST.
template <class F>
inline double CountPaths(const F &fst) {
  if (fst->Properties(kConnectedAndTopSorted, false)
      == kConnectedAndTopSorted) {
    VLOG(3) << "CountPaths(const Fst &): already connected and top. sorted";
    return CountPathsTopSorted(fst);
  }
  // Function is templated on FST type F to take advantage of overloading of
  // VectorFst ctor to handle copying of a VectorFst more efficiently.
  fst::VectorFst<typename F::Arc> vector_fst(fst);
  return CountPaths(&vector_fst);
}

}  // namespace festus

#endif  // FESTUS_RUNTIME_FST_UTIL_H__

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

#ifndef FESTUS_MODE_SEARCH_H__
#define FESTUS_MODE_SEARCH_H__

#include <cmath>
#include <cstddef>
#include <ios>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <fst/arc-map.h>
#include <fst/arc.h>
#include <fst/arcsort.h>
#include <fst/compat.h>
#include <fst/compose.h>
#include <fst/expanded-fst.h>
#include <fst/fst.h>
#include <fst/mutable-fst.h>
#include <fst/project.h>
#include <fst/prune.h>
#include <fst/queue.h>
#include <fst/randgen.h>
#include <fst/rmepsilon.h>
#include <fst/shortest-distance.h>
#include <fst/shortest-path.h>
#include <fst/string.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>

#include "festus/weight-convert.h"

namespace festus {

// Converts the given FST to the tropical semiring, finds the shortest path, and
// returns it as an unweighted FST in the original semiring. Uses the
// NaturalShortestFirstQueue for the tropical semiring. This is admissible if
// the FST is a locally normalized stochastic FST.
template <class Arc>
void TropicalShortestFirstPath(const fst::Fst<Arc> &fst,
                               fst::MutableFst<Arc> *path,
                               float shortest_delta = fst::kShortestDelta) {
  using ValueType = typename Arc::Weight::ValueType;
  using MinWeight = fst::TropicalWeightTpl<ValueType>;
  using MinArc = fst::ArcTpl<MinWeight>;
  using Mapper = fst::WeightConvertMapper<Arc, MinArc>;
  using StateQueue = fst::NaturalShortestFirstQueue<
    typename MinArc::StateId, MinWeight>;
  using ArcFilter = fst::AnyArcFilter<MinArc>;
  fst::ArcMapFst<Arc, MinArc, Mapper> min_fst(fst, Mapper());
  std::vector<MinWeight> distance;
  StateQueue state_queue(distance);
  ArcFilter arc_filter;
  fst::ShortestPathOptions<MinArc, StateQueue, ArcFilter> options(
      &state_queue, arc_filter);
  options.delta = shortest_delta;
  options.first_path = true;
  fst::VectorFst<MinArc> min_path;
  fst::ShortestPath(min_fst, &min_path, &distance, options);
  fst::ArcMap(min_path, path, fst::RmWeightMapper<MinArc, Arc>());
}

// Converts the given FST to the tropical semiring, prunes it, and updates it in
// the original semiring.
template <class Arc>
void TropicalPrune(fst::MutableFst<Arc> *fst,
                   typename Arc::Weight::ValueType weight_threshold,
                   int state_threshold = fst::kNoStateId) {
  using ValueType = typename Arc::Weight::ValueType;
  using MinWeight = fst::TropicalWeightTpl<ValueType>;
  using MinArc = fst::ArcTpl<MinWeight>;
  // TODO: If weights are cast-convertible, use Cast instead of ConvertWeight.
  fst::VectorFst<MinArc> min_fst;
  ConvertWeight(*fst, &min_fst);
  fst::Prune(&min_fst, MinWeight(weight_threshold), state_threshold);
  ConvertWeight(min_fst, fst);
}

template <class Arc>
double LogNumPaths(const fst::Fst<Arc> &fst) {
  using LnArc = fst::Log64Arc;
  using Mapper = fst::RmWeightMapper<Arc, LnArc>;
  std::unique_ptr<fst::VectorFst<Arc>> copy;
  const fst::Fst<Arc> *connected_fst;
  static constexpr auto kConnected = fst::kAccessible | fst::kCoAccessible;
  if (fst.Properties(kConnected, false) != kConnected) {
    copy.reset(new fst::VectorFst<Arc>(fst));
    fst::Connect(copy.get());
    connected_fst = copy.get();
  } else {
    connected_fst = &fst;
  }
  if (connected_fst->Properties(fst::kCyclic, true) == fst::kCyclic) {
    return INFINITY;
  }
  using LnFst = fst::ArcMapFst<Arc, LnArc, Mapper>;
  const auto total = fst::ShortestDistance(LnFst(*connected_fst, Mapper()));
  return -total.Value();
}

// Returns exp(ln_value) as a string.
inline std::string StrExp(double ln_value) {
  // TODO: Handle large negative values.
  static constexpr int16 kMaxFinite = 709;
  std::ostringstream strm;
  if (std::isfinite(ln_value) && ln_value > kMaxFinite) {
    static constexpr double kLn10 = 1.0 / std::log(10.0);
    double log10_value = ln_value * kLn10;
    static constexpr int64 kMaxExponent = 1LL << 53;
    if (log10_value > kMaxExponent) {
      strm << "pow(10, " << log10_value << ")";
    } else {
      const int64 exponent = static_cast<int64>(log10_value);
      log10_value -= exponent;
      strm << std::pow(10.0, log10_value) << "e" << exponent;
    }
  } else {
    strm << std::exp(ln_value);
  }
  return strm.str();
}

template <class Arc>
std::string CountPaths(const fst::Fst<Arc> &fst) {
  return StrExp(LogNumPaths(fst));
}

template <class Arc>
std::string FstSizeInfo(const fst::Fst<Arc> &fst) {
  std::ostringstream strm;
  strm << "FST has " << fst::CountStates(fst) << " states, "
       << fst::CountArcs(fst) << " arcs, and " << CountPaths(fst) << " paths";
  return strm.str();
}

template <class Arc>
fst::VectorFst<Arc> CollapseRunsRmBlank(
    const fst::SymbolTable *symbols,
    const std::string &blank = "blank",
    const std::string &pad = "pad") {
  CHECK_GE(symbols->Find(blank), 1);
  CHECK_GE(symbols->Find(pad), 1);
  std::map<int, std::size_t> label2state;
  label2state[symbols->Find(blank)] = 0;
  label2state[symbols->Find(pad)] = 0;
  std::size_t num_states = 1;
  using STI = fst::SymbolTableIterator;
  for (STI iter(*symbols); !iter.Done(); iter.Next()) {
    const auto &symbol = iter.Symbol();
    const auto label = iter.Value();
    if (symbol == blank || symbol == pad || label == 0) continue;
    label2state[label] = num_states++;
  }
  static constexpr auto one = Arc::Weight::One();
  fst::VectorFst<Arc> fst;
  fst.SetInputSymbols(symbols);
  fst.SetOutputSymbols(symbols);
  fst.ReserveStates(num_states);
  for (std::size_t i = 0; i < num_states; ++i) {
    const auto state = fst.AddState();
    fst.SetFinal(state, one);
  }
  fst.SetStart(0);
  for (std::size_t state = 0; state < num_states; ++state) {
    for (STI iter(*symbols); !iter.Done(); iter.Next()) {
      const auto ilabel = iter.Value();
      if (ilabel == 0) continue;
      const std::size_t nextstate = label2state[ilabel];
      const int olabel = (nextstate == state || nextstate == 0) ? 0 : ilabel;
      fst.EmplaceArc(state, ilabel, olabel, one, nextstate);
    }
  }
  fst::ArcSort(&fst, fst::ILabelCompare<Arc>());
  return fst;
}

// Search for the mode of the distribution (A o B) given a stochastic FSA A and
// an unweighted functional FST B.
template <class Arc>
class ModeSearch {
 public:
  void SetShortestDelta(float delta) { shortest_delta_ = delta; }

  void SetName(const std::string &name) { name_ = name; }

  void SetA(const fst::Fst<Arc> *fst) { fst_a_ = fst; }

  void SetB(const fst::Fst<Arc> *fst) { fst_b_ = fst; }

  std::string FindModeBySampling(int max_draws, double theta) {
    ResetSearch();
    fst::VectorFst<Arc> fst;
    TropicalShortestFirstPath(*fst_a_, &fst, shortest_delta_);
    PathToLabeling(&fst);
    best_path_labeling_ = LabelingToString(fst);
    if (max_draws <= 0) {
      VLOG(1) << "Stopping: random sampling not requested";
      Summary(0);
      return best_path_labeling_;
    }
    IncrementCount(best_path_labeling_);
    AddProbability(best_path_labeling_, fst);
    if (max_ > 1 - total_probability_) {
      VLOG(1) << "Stopping: true mode found";
      return Summary(0);
    }
    int draws = 0;
    while (draws < max_draws) {
      ++draws;
      fst::RandGen(*fst_a_, &fst, rand_gen_options_);
      PathToLabeling(&fst);
      std::string labeling = LabelingToString(fst);
      const int count = IncrementCount(labeling);
      CHECK_GE(count, 1);
      if (ComputeProbability(count, draws, theta)) {
        if (AddProbability(labeling, fst)) {
          if (max_ > 1 - total_probability_) {
            VLOG(1) << "Stopping: true mode found";
            return Summary(draws);
          }
        }
      }
      const double p = std::pow(1 - max_, draws + 1) -
                       std::pow(total_probability_, draws + 1);
      if (p < theta) {
        VLOG(1) << "Stopping: approximate with p-value " << p;
        return Summary(draws);
      }
    }
    VLOG(1) << "Stopping: max_draws reached";
    return Summary(draws);
  }

 private:
  void ResetSearch() {
    count_.clear();
    probability_.clear();
    best_path_labeling_.clear();
    argmax_.clear();
    max_ = 0;
    total_probability_ = 0;
    probability_computations_ = 0;
  }

  std::string Summary(int draws) const {
    std::string argmax_count;
    int max_count = 0;
    for (const auto &pair : count_) {
      if (pair.second > max_count) {
        argmax_count = pair.first;
        max_count = pair.second;
      }
    }
    VLOG(1) << "Best-path labeling\t" << name_ << "\t" << best_path_labeling_;
    VLOG(1) << "Most frequent labeling\t" << name_ << "\t" << argmax_count;
    VLOG(1) << "Most likely labeling\t" << name_ << "\t" << argmax_;
    VLOG(1) << "Probability of most likely labeling: " << max_;
    VLOG(1) << "Total probability observed: " << total_probability_;
    VLOG(1) << "Paths sampled: " << draws;
    VLOG(1) << "Probability computations: " << probability_computations_;
    VLOG(1) << "Most likely labeling corresponds to best-path labeling: "
            << std::boolalpha << (argmax_ == best_path_labeling_);
    VLOG(1) << "Most likely labeling corresponds to most frequent labeling: "
            << std::boolalpha << (argmax_ == argmax_count);
    return argmax_;
  }

  void PathToLabeling(fst::VectorFst<Arc> *fst) const {
    fst::VectorFst<Arc> composed;
    fst::Compose(*fst, *fst_b_, &composed);
    fst::Project(&composed, fst::PROJECT_OUTPUT);
    fst::RmEpsilon(&composed);
    *fst = composed;
    fst::ArcMap(fst, fst::RmWeightMapper<Arc>());
    fst::ArcSort(fst, fst::ILabelCompare<Arc>());
  }

  std::string LabelingToString(const fst::Fst<Arc> &fst) const {
    fst::StringPrinter<Arc> print(fst::SYMBOL, fst.OutputSymbols());
    std::string str;
    CHECK(print(fst, &str));
    return str;
  }

  double LabelingProbability(const fst::Fst<Arc> &labeling) const {
    CHECK_EQ(fst::kUnweighted, labeling.Properties(fst::kUnweighted, false));
    CHECK_EQ(fst::kILabelSorted,
             labeling.Properties(fst::kILabelSorted, false));
    fst::VectorFst<Arc> composed2;
    fst::Compose(*fst_b_, labeling, &composed2);
    fst::ArcSort(&composed2, fst::ILabelCompare<Arc>());
    fst::ComposeFst<Arc> composed(*fst_a_, composed2);
    if (FLAGS_v >= 2) {
      const auto s = fst::CountStates(labeling) - 1;
      // TODO: Only holds when FST B was constructed with CollapseRunsRmBlank.
      VLOG(2) << "(B o Labeling) FST will have " << 2 * s + 1 << " states "
              << "and no more than " << 7 * s + 1 << " arcs";
      VLOG(2) << "(B o Labeling) " << FstSizeInfo(composed2);
      VLOG(2) << "Labeling Probability " << FstSizeInfo(composed);
    }
    const auto total = fst::ShortestDistance(composed, shortest_delta_);
    return std::exp(-total.Value());
  }

  int IncrementCount(const std::string &str) {
    auto iter = count_.find(str);
    if (iter != count_.end()) {
      return ++iter->second;
    } else {
      return count_[str] = 1;
    }
  }

  bool AddProbability(const std::string &str, const fst::Fst<Arc> &fst) {
    if (probability_.find(str) != probability_.end()) return false;
    ++probability_computations_;
    const double prob = LabelingProbability(fst);
    CHECK_GE(prob, 0);
    CHECK_LE(prob, 1);
    probability_[str] = prob;
    if (prob > max_) {
      argmax_ = str;
      max_ = prob;
    }
    total_probability_ += prob;
    return true;
  }

  bool ComputeProbability(int count, int draws, double theta) const {
    return count > 1;
  }

  using Selector = fst::FastLogProbArcSelector<Arc>;
  const Selector selector_{};
  const fst::RandGenOptions<Selector> rand_gen_options_{selector_};

  float shortest_delta_ = 1e-9;

  std::string name_;
  const fst::Fst<Arc> *fst_a_;  // pointer not owned
  const fst::Fst<Arc> *fst_b_;  // pointer not owned

  std::map<std::string, int> count_;
  std::map<std::string, double> probability_;
  std::string best_path_labeling_;
  std::string argmax_;
  double max_;
  double total_probability_;
  int probability_computations_;
};

}  // namespace festus

#endif  // FESTUS_MODE_SEARCH_H__

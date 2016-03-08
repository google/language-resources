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
// Copyright 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Self-contained grapheme-to-phoneme (G2P) inference library.

#ifndef FESTUS_RUNTIME_G2P_H__
#define FESTUS_RUNTIME_G2P_H__

#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include <fst/compat.h>
#include <fst/fstlib.h>

// NB: Non-standard include directive to facilitate stand-alone compilation.
#include "fst-util.h"

namespace festus {

struct G2PResult {
  // A list of pronunciations and their associated posterior probabilities,
  // in descending order.
  std::vector<std::pair<string, float>> pronunciations;

  // The number of viable hypotheses in the marginal posterior distribution.
  double num_hypotheses;

  // A diagnostic message that is set if G2P<>::Pronounce() failed.
  string error;
};

struct G2POptions {
  static constexpr std::size_t kDefaultMaxProns = 3;
  static constexpr float kDefaultPruningThreshold = 0.5f;

  // Upper limit on the number of pronunciations that will be computed by
  // G2P<>::Pronounce().
  std::size_t max_prons = kDefaultMaxProns;

  // Threshold used for pruning during decoding when max_prons > 1.
  //
  // This is a number in [0;1] which factors into the decision whether to keep
  // or prune away alternative hypotheses beyond the leading hypothesis. If the
  // probability of the leading hypothesis is p and the real pruning threshold
  // is theta, then hypotheses whose probability is less than p * theta will be
  // discarded.
  //
  // Has no effect when max_prons == 1.
  float real_pruning_threshold = kDefaultPruningThreshold;

  // Convergence parameter for FST operations.
  float delta = fst::kDelta;
};

template <class Arc>
class G2P {
 public:
  typedef fst::Fst<Arc> Lattice;
  typedef fst::VectorFst<Arc> MutableLattice;
  typedef fst::CompactFst<Arc, fst::StringCompactor<Arc>> StringFst;

  G2P() = default;
  ~G2P() = default;

  // These setters are used to specify the factors of the G2P model.
  void SetBytesToGraphonesFst(std::unique_ptr<const Lattice> fst);
  void SetGraphoneModelFst(std::unique_ptr<const Lattice> fst);
  void SetPhonemesToGraphonesFst(std::unique_ptr<const Lattice> fst);

  // Finds pronunciations for the given spelling and configured G2P model.
  //
  // Aguments:
  // - spelling: an orthographic word
  // - result: a valid pointer to a G2PResult structure where
  //           pronunciations and other information will be stored
  // - opts: options for controlling the inference procedure; see G2POptions
  //
  // Returns true on succces. Returns false on error and sets result->error.
  //
  // On success, result->pronunciations contains pronunciations and their
  // corresponding posterior probabilities under the model. At most
  // opts.max_prons pronunciations are computed.
  bool Pronounce(const string &spelling,
                 G2PResult *result,
                 const G2POptions &opts = G2POptions()) const;

 private:
  std::unique_ptr<const Lattice> bytes_to_graphones_;
  std::unique_ptr<const Lattice> graphone_model_;
  std::unique_ptr<const Lattice> phonemes_to_graphones_;
  bool bytes_to_graphones_is_insertion_free_ = false;
  bool phonemes_to_graphones_is_insertion_free_ = false;
};

template <class Arc>
void G2P<Arc>::SetGraphoneModelFst(std::unique_ptr<const Lattice> fst) {
  graphone_model_ = std::move(fst);
}

template <class Arc>
void G2P<Arc>::SetBytesToGraphonesFst(std::unique_ptr<const Lattice> fst) {
  bytes_to_graphones_ = std::move(fst);
  if (bytes_to_graphones_->Properties(fst::kNoIEpsilons, true)) {
    bytes_to_graphones_is_insertion_free_ = true;
  } else {
    // Test for the presence of unanchored insertion loops by checking if
    // the epsilon-graph has cycles.
    std::vector<typename Arc::StateId> unused_order;
    fst::TopOrderVisitor<Arc> top_order_visitor(
        &unused_order, &bytes_to_graphones_is_insertion_free_);
    fst::DfsVisit(*bytes_to_graphones_, &top_order_visitor,
                  fst::InputEpsilonArcFilter<Arc>());
  }
  if (FLAGS_v >= 1) {
    LOG(INFO) << "bytes_to_graphones is"
              << (bytes_to_graphones_is_insertion_free_ ? " " : " NOT ")
              << "graphone-insertion-free";
  }
}

template <class Arc>
void G2P<Arc>::SetPhonemesToGraphonesFst(std::unique_ptr<const Lattice> fst) {
  phonemes_to_graphones_ = std::move(fst);
  if (phonemes_to_graphones_->Properties(fst::kNoOEpsilons, true)) {
    phonemes_to_graphones_is_insertion_free_ = true;
  } else {
    // Test for the presence of unanchored insertion loops by checking if
    // the epsilon-graph has cycles.
    std::vector<typename Arc::StateId> unused_order;
    fst::TopOrderVisitor<Arc> top_order_visitor(
        &unused_order, &phonemes_to_graphones_is_insertion_free_);
    fst::DfsVisit(*phonemes_to_graphones_, &top_order_visitor,
                  fst::OutputEpsilonArcFilter<Arc>());
  }
  if (FLAGS_v >= 1) {
    LOG(INFO) << "phonemes_to_graphones is"
              << (phonemes_to_graphones_is_insertion_free_ ? " " : " NOT ")
              << "phoneme-insertion-free";
  }
}

template <class Arc>
bool G2P<Arc>::Pronounce(const string &spelling,
                         G2PResult *result,
                         const G2POptions &opts) const {
  typedef typename Arc::Weight Weight;

  if (0 == opts.max_prons) {
    result->pronunciations.clear();
    result->num_hypotheses = 0;
    result->error.clear();
    return true;
  }

  VLOG(2) << "1. Turn spelling string into FST.";
  const unsigned char *begin =
      reinterpret_cast<const unsigned char *>(spelling.data());
  StringFst spelling_fst;
  spelling_fst.SetCompactElements(begin, begin + spelling.size());
  VLOG_PROPERTIES(3, spelling_fst);

  VLOG(2) << "2. Reverse-project (inject) spelling FST into graphone lattice.";
  MutableLattice lattice;
  ComposeProjectRmEpsilon(
      spelling_fst, *bytes_to_graphones_, fst::PROJECT_OUTPUT, &lattice,
      opts.delta, bytes_to_graphones_->Properties(fst::kNoIEpsilons, false));
  if (fst::kNoStateId == lattice.Start()) {
    result->error = "Could not create graphone lattice from spelling";
    return false;
  }
  if (bytes_to_graphones_is_insertion_free_) {
    // Since the spelling_fst is a string and therefore acyclic, and since the
    // bytes_to_graphones FST is insertion-free, the resulting graphone lattice
    // must be acyclic.
    ExpectProperties(lattice, fst::kAcyclic);
  }
  VLOG_PROPERTIES(3, lattice);

  VLOG(2) << "3. Intersect graphone lattice with graphone model.";
  MutableLattice lattice2;
  PhiCompose(lattice, *graphone_model_, 0, &lattice2);
  if (fst::kNoStateId == lattice2.Start()) {
    result->error = "Could not rescore graphone lattice";
    return false;
  }
  if (bytes_to_graphones_is_insertion_free_) {
    // Since the raw graphone lattice was acyclic (per above), the rescored
    // graphone lattice must also be acyclic.
    ExpectProperties(lattice2, fst::kAcyclic);
  }
  VLOG_PROPERTIES(3, lattice2);

  VLOG(2) << "4. Project graphone lattice into phoneme lattice.";
  ComposeProjectRmEpsilon(
      *phonemes_to_graphones_, lattice2, fst::PROJECT_INPUT, &lattice,
      opts.delta);
  if (fst::kNoStateId == lattice.Start()) {
    result->error = "Could not create phoneme lattice";
    return false;
  }
  if (bytes_to_graphones_is_insertion_free_ &&
      phonemes_to_graphones_is_insertion_free_) {
    // Since the rescored graphone lattice was acyclic (per above), and since
    // the phonemes_to_graphones FST is insertion-free on the phoneme side, the
    // resulting phoneme lattice must be acyclic.
    ExpectProperties(lattice, fst::kAcyclic);
  }
  VLOG_PROPERTIES(3, lattice);

  VLOG(2) << "5. Compute normalizing total of the marginal posterior lattice.";
  Weight total_weight = fst::ShortestDistance(lattice, opts.delta);
  if (Weight::Zero() == total_weight) {
    LOG(WARNING) << "Cannot normalize the posterior distribution";
    total_weight = Weight::One();
  }

  VLOG(2) << "6. Convert posterior lattice to tropical semiring for decoding.";
  fst::StdVectorFst std_lattice;
  DeterminizeConvertWeight(lattice, &std_lattice, opts.delta);
  result->num_hypotheses = CountPaths(&std_lattice);
  if (bytes_to_graphones_is_insertion_free_ &&
      phonemes_to_graphones_is_insertion_free_) {
    // Since the phoneme lattice was acyclic (per above), the converted lattice
    // must also be acyclic. In fact after CountPaths() it must in fact be
    // topologically sorted.
    ExpectProperties(std_lattice, fst::kAcyclic | fst::kTopSorted);
  }
  VLOG_PROPERTIES(3, std_lattice);

  VLOG(2) << "7. Decode shortest paths.";
  fst::StdVectorFst paths;
  if (opts.max_prons > 1) {
    fst::ShortestPath(std_lattice, &paths, opts.max_prons, false, false,
                      -std::log(opts.real_pruning_threshold));
  } else {
    fst::ShortestPath(std_lattice, &paths);
  }

  VLOG(2) << "8. Convert shortest paths to pronunciations.";
  PathsToVector(paths, &result->pronunciations, total_weight.Value());
  result->error.clear();
  return true;
}

}  // namespace festus

#endif  // FESTUS_RUNTIME_G2P_H__

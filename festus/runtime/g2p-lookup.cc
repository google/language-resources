// festus/runtime/g2p-lookup.cc
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
// Command-line interface for grapheme-to-phoneme (G2P) pronunciation lookup.

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <fst/extensions/ngram/ngram-fst.h>

// NB: Non-standard include directive to facilitate stand-alone compilation.
#include "compact.h"
#include "g2p.h"

typedef fst::LogArc MyArc;
typedef festus::G2P<MyArc> MyG2P;

namespace {

// Typically the bytes_to_graphones and phonemes_to_graphones FSTs can be
// stored compactly with arcs represented as follows:
//
// - use 8 bits (255 possible values) for the input label (byte or phoneme);
// - use 10 bits (1023 possible values) for the output label (graphone);
// - no need to store weights, as these machines are typically unweighted;
// - use 14 bits (16383 possible values) for the target state of an arc.
//
// Register the corresponding compact FST type so that the generic Read()
// operation (in the body of ReadFst) can recognize and load such FSTs.
static fst::FstRegisterer<festus::Compact_8_10_0_14_Fst<MyArc>> compact_reg;

// The graphones_model FST is a backoff language model in OpenGrm format.
// Register the LOUDS-compressed FST representation here, for use with Read().
static fst::FstRegisterer<fst::NGramFst<MyArc>> ngram_reg;

inline std::unique_ptr<const MyG2P::Lattice> ReadFst(const string &path) {
  // TODO: std::make_unique() in C++14.
  return std::unique_ptr<const MyG2P::Lattice>(MyG2P::Lattice::Read(path));
}

}  // namespace

static const char kUsage[] =
    R"(Grapheme-to-phoneme (G2P) pronunciation lookup.

Reads orthographic words (one word per line) from a file or from stdin
and writes predicted pronunciations to stdout. The output on stdout is in
tab-separated value (TSV) format with the following four columns:

  1. the orthographic word,
  2. its pronunciation,
  3. the marginal posterior probability of the pronunciation, and
  4. the cumulative probability of the current and preceding pronunciations.

When multiple pronunciations are generated, they are output in decending order
of posterior probability.

The grapheme-to-phoneme model is specified by the three flags
--bytes_to_graphones, --graphone_model, and --phonemes_to_graphones
corresponding to the factorization of the model.

The flag --max_prons specifies the upper limit on the number of
pronunciations that will be generated for a given word.

The --real_pruning_threshold controls theta the amount of pruning that will be
applied to the marginal posterior lattice during decoding when max_prons > 1.
A hypotheses will be pruned away if its probability is less than theta times
the probability of the most likely hypothesis.

Usage:
  g2p-lookup [--flags...] [WORDS_FILE]
)";

DEFINE_string(bytes_to_graphones, "", "Path to bytes_to_graphones FST");
DEFINE_string(graphone_model, "", "Path to graphone_model FST");
DEFINE_string(phonemes_to_graphones, "", "Path to phonemes_to_graphones FST");

DEFINE_int32(max_prons,
             festus::G2POptions::kDefaultMaxProns,
             "Maximal number of pronunciations per word");
DEFINE_double(real_pruning_threshold,
              festus::G2POptions::kDefaultPruningThreshold,
              "Real pruning threshold; a number between 0 (no pruning) and 1");
DEFINE_double(delta,
              fst::kDelta,
              "Convergence threshold for FST operations");

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);

  auto bytes_to_graphones = ReadFst(FLAGS_bytes_to_graphones);
  if (!bytes_to_graphones) return 2;

  auto graphone_model = ReadFst(FLAGS_graphone_model);
  if (!graphone_model) return 2;

  auto phonemes_to_graphones = ReadFst(FLAGS_phonemes_to_graphones);
  if (!phonemes_to_graphones) return 2;

  MyG2P g2p;
  g2p.SetBytesToGraphonesFst(std::move(bytes_to_graphones));
  g2p.SetGraphoneModelFst(std::move(graphone_model));
  g2p.SetPhonemesToGraphonesFst(std::move(phonemes_to_graphones));

  festus::G2POptions options;
  options.max_prons = FLAGS_max_prons;
  options.real_pruning_threshold = FLAGS_real_pruning_threshold;
  options.delta = FLAGS_delta;

  festus::G2PResult result;
  bool success = true;
  for (string word; std::getline(std::cin, word); /*empty*/) {
    if (g2p.Pronounce(word, &result, options)) {
      VLOG(1) << result.num_hypotheses
              << " hypothes" << (result.num_hypotheses == 1 ? "is" : "es")
              << " searched";
      auto num_prons = result.pronunciations.size();
      VLOG(1) << num_prons
              << " pronunciation" << (num_prons == 1 ? "" : "s")
              << " found";
      double cumul = 0;
      for (const auto &p : result.pronunciations) {
        cumul += p.second;
        std::cout << word << "\t" << p.first << "\t" << p.second
                  << "\t" << cumul << std::endl;
      }
    } else {
      LOG(ERROR) << "No prounciations found for " << word
                 << ": " << result.error;
      success = false;
    }
  }
  return success ? 0 : 1;
}

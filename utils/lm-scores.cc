// utils/lm-scores.cc
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
// Command-line interface for scoring text with codepoint LMs.

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <fst/extensions/ngram/ngram-fst.h>

#include "festus/label-maker.h"
#include "festus/runtime/fst-util.h"

namespace {

// Register the LOUDS-compressed n-gram FST representation.
static fst::FstRegisterer<fst::NGramFst<fst::StdArc>> std_ngram_reg;

}  // namespace

static const char kUsage[] =
    R"(Scores text with codepoint LMs.

Reads text line by line from stdin and scores it with the language models
given on the command line.

Writes tab-spearated text to stdout, consisting of the line, followed by pairs
of (model name, score) sorted by increasing score (decreasing likelihood).

Usage:
  lm-scores [LM.fst...]
)";

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);

  std::vector<std::pair<string, std::unique_ptr<fst::StdFst>>> lms;
  for (int i = 1; i < argc; ++i) {
    std::unique_ptr<fst::StdFst> lm(fst::StdFst::Read(argv[i]));
    if (!lm) return 2;
    lms.emplace_back(argv[i], std::move(lm));
  }

  festus::UnicodeLabelMaker label_maker;
  festus::LabelMaker::Labels labels;

  for (string line; std::getline(std::cin, line); /*empty*/) {
    if (label_maker.StringToLabels(line, &labels)) {
      fst::StdCompactStringFst string_fst;
      string_fst.SetCompactElements(labels.begin(), labels.end());
      std::vector<std::pair<float, string>> scores;
      for (const auto &lm : lms) {
        auto scored = festus::PhiComposeFst(string_fst, *lm.second, 0);
        auto total = fst::ShortestDistance(scored);
        scores.emplace_back(total.Value(), lm.first);
      }
      std::sort(scores.begin(), scores.end());
      std::cout << line;
      for (const auto &score : scores) {
        std::cout << "\t" << score.second << "\t" << score.first;
      }
      std::cout << std::endl;
    } else {
      std::cerr << "Could not compute labels for line: " << line << std::endl;
    }
  }

  return 0;
}

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
// Command-line utility for finding the best labeling as the mode of an
// implicit composition of an SFST and an unweighted functional FST.

#include <iostream>
#include <memory>
#include <string>

#include <fst/arc.h>
#include <fst/compat.h>
#include <fst/flags.h>
#include <fst/fst.h>
#include <fst/vector-fst.h>

#include "festus/mode-search.h"

const char kUsage[] =
    R"(Finds the best labeling as a mode of an SFST composed with an unweighted
FST that collapses contiguous symbol runs and removes blank/padding symbols.

Usage:
  best-labeling [in.fst]...
)";

DEFINE_double(delta, 1e-9, "Delta for ShortestPath/ShortestDistance");
DEFINE_double(theta, 0.01, "Confidence level for sampling strategy");
DEFINE_int32(max_draws, 1000, "Maximum sample size");

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc < 2) {
    ShowUsage();
    return 2;
  }

  using LnArc = fst::Log64Arc;
  using LnFst = fst::Fst<LnArc>;
  festus::ModeSearch<LnArc> mode_search;
  mode_search.SetShortestDelta(FLAGS_delta);
  std::unique_ptr<LnFst> fst_a;
  fst::VectorFst<LnArc> fst_b;

  int errors = 0;
  for (int i = 1; i < argc; ++i) {
    const std::string path = argv[i];
    fst_a.reset(LnFst::Read(path));
    if (!fst_a) {
      ++errors;
      continue;
    }
    mode_search.SetName(path);
    mode_search.SetA(fst_a.get());
    CHECK(fst_a->InputSymbols());
    if (!fst_b.InputSymbols()
        || fst_b.InputSymbols()->LabeledCheckSum()
        != fst_a->InputSymbols()->LabeledCheckSum()) {
      fst_b = festus::CollapseRunsRmBlank<LnArc>(fst_a->InputSymbols());
      mode_search.SetB(&fst_b);
    }
    std::cout << path << std::endl;
    const auto &best_labeling = mode_search.FindModeBySampling(
        FLAGS_max_draws, FLAGS_theta);
    std::cout << best_labeling << std::endl << std::endl;
  }

  return errors ? 1 : 0;
}

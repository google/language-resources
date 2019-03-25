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
// Command-line utility for conflating epsilon-cycles in an FST.

#include <cstring>
#include <memory>
#include <string>

#include <fst/arc.h>
#include <fst/compat.h>
#include <fst/flags.h>
#include <fst/fst.h>
#include <fst/shortest-distance.h>
#include <fst/vector-fst.h>

#include "festus/rmepscycle.h"

const char kUsage[] =
    R"(Conflates epsilon-cycles in an FST.

Usage:
  fstrmepscycle [in.fst [out.fst]]
)";

DEFINE_double(delta, fst::kShortestDelta, "Delta for ShortestPath convergence");

inline string FstName(int argc, char *argv[], int pos) {
  return (argc > pos && std::strcmp(argv[1], "-") != 0) ? argv[pos] : "";
}

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 3) {
    ShowUsage();
    return 2;
  }
  const std::string in_name = FstName(argc, argv, 1);
  const std::string out_name = FstName(argc, argv, 2);

  // TODO: Use the framework in fst::script to dispatch on the Arc type.
  using MyArc = fst::Log64Arc;
  std::unique_ptr<fst::Fst<MyArc>> in_fst(fst::Fst<MyArc>::Read(in_name));
  if (!in_fst) return 1;

  fst::VectorFst<MyArc> out_fst(*in_fst);
  festus::RmEpsilonCycle(&out_fst, FLAGS_delta);
  out_fst.Write(out_name);

  return 0;
}

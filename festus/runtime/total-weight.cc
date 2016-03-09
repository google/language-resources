// festus/runtime/total-weight.cc
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
// Prints the total weight of an FST. Used for checking that an n-gram model
// is properly normalized and usable with older versions of OpenFst < 1.5.0.
//
// This is essentially the Forward algorithm combined with on-the-fly
// conversion to the Log64 semiring and, if requested, on-the-fly phi-removal.

#include <cmath>
#include <cstring>
#include <iostream>
#include <memory>
#include <utility>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <fst/extensions/ngram/ngram-fst.h>

// NB: Non-standard include directive to facilitate stand-alone compilation.
#include "fst-util.h"

static const char kUsage[] =
    R"(Prints the total weight (in the log semiring) of an FST to stdout.

Usage:
  total-weight [--flags...] [FST]
)";

DEFINE_string(arc_type, "std", "arc type of the input FST; supported values "
              "are: 'std' (default), 'log', and 'log64'");
DEFINE_int32(phi_label, 0, "Phi (failure, backoff) label");
DEFINE_double(convergence_delta, 1e-12,
              "Convergence parameter for shortest distance");
DEFINE_double(comparison_delta, 1e-6, "Comparison delta");

namespace {

static fst::FstRegisterer<fst::NGramFst<fst::StdArc>> std_ngram_reg;
static fst::FstRegisterer<fst::NGramFst<fst::LogArc>> log_ngram_reg;
static fst::FstRegisterer<fst::NGramFst<fst::Log64Arc>> log64_ngram_reg;

template <class Arc>
int PrintTotalWeight(const string &path) {
  std::unique_ptr<fst::Fst<Arc>> fst(fst::Fst<Arc>::Read(path));
  if (!fst) return 2;

  std::unique_ptr<fst::Fst<Arc>> fst2;
  if (FLAGS_phi_label == fst::kNoLabel) {
    VLOG(1) << "Skipping phi removal, computing weight directly";
    fst2 = std::move(fst);
  } else {
    VLOG(1) << "Computing weight on phi-removed FST";
    fst2.reset(festus::RmPhiFst(*fst, FLAGS_phi_label).Copy());
  }

  typedef fst::WeightConvertMapper<Arc, fst::Log64Arc> ToLog;
  fst::ArcMapFst<Arc, fst::Log64Arc, ToLog> log_fst(*fst2, ToLog());

  auto sum = fst::ShortestDistance(log_fst, FLAGS_convergence_delta);

  std::cout << "Total weight w = " << sum.Value() << std::endl;
  std::cout << "       exp(-w) = " << std::exp(-sum.Value()) << std::endl;

  bool eq1 = fst::ApproxEqual(sum, fst::Log64Arc::Weight::One(),
                              FLAGS_comparison_delta);
  if (eq1) {
    std::cerr << "PASS" << std::endl;
    return 0;
  } else {
    std::cerr << "FAIL" << std::endl;
    return 1;
  }
}

}  // namespace

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 2) {
    ShowUsage();
    return 2;
  }

  string in_name = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";

  if (FLAGS_arc_type == "std") {
    return PrintTotalWeight<fst::StdArc>(in_name);
  } else if (FLAGS_arc_type == "log") {
    return PrintTotalWeight<fst::LogArc>(in_name);
  } else if (FLAGS_arc_type == "log64") {
    return PrintTotalWeight<fst::Log64Arc>(in_name);
  } else {
    LOG(ERROR) << "Unable to handle requested arc type: " << FLAGS_arc_type;
    return 2;
  }
}

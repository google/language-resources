// festus/make-runtime-fsts.cc
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
// Creates canonical projection/injection FSTs in compact runtime format.

#include <cstring>
#include <iostream>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <fst/script/info-impl.h>

#include "festus/alignables-util.h"
#include "festus/iterator.h"
#include "festus/types.h"
#include "festus/runtime/compact.h"
#include "festus/runtime/fst-util.h"

namespace {

template <class Arc>
void PrintInfo(const fst::Fst<Arc> &fst, bool test_properties=false) {
  fst::FstInfo info(fst, test_properties);
  fst::PrintFstInfoImpl(info, true /* pipe, i.e. print to stderr */);
}

template <class Arc>
void Compactify(const fst::VectorFst<Arc> &fst,
                const string &path) {
  typedef festus::Compactor_8_10_0_14<fst::LogArc> MyCompactor;
  for (auto s : festus::States(fst)) {
    for (const auto &arc : festus::Arcs(fst, s)) {
      if (arc.ilabel < 0 || arc.ilabel >= MyCompactor::kMaxILabel) {
        LOG(ERROR) << "ilabel out of range: " << arc.ilabel;
      }
      if (arc.olabel < 0 || arc.olabel >= MyCompactor::kMaxOLabel) {
        LOG(ERROR) << "olabel out of range: " << arc.olabel;
      }
      if (arc.nextstate < 0 || arc.nextstate >= MyCompactor::kMaxState) {
        LOG(ERROR) << "nextstate out of range: " << arc.nextstate;
      }
    }
  }
  fst::VectorFst<fst::LogArc> log_fst;
  festus::ConvertWeight(fst, &log_fst);
  festus::LogCompact_8_10_0_14_Fst compact_fst(log_fst);
  PrintInfo(compact_fst);
  compact_fst.Write(path);
}

}  // namespace

static const char kUsage[] =
    R"(Creates canonical projection/injection FSTs in compact runtime format.
The alignables specification (configured with the flag --alignables) defines
a factorized model consisting of latent chunked alignment pairs and two
canonical projection functions that map sequences of alignment pairs to
input and output sequences, respectively.

This tool writes the two canonical projection/injection FSTs to the files
specified on the command line. The FSTs are written in a compact representation
and using Log arcs, for direct use with the runtime inference library.

Usage:
  make-runtime-fsts --alignables=spec.txt input_to_pair.fst output_to_pair.fst
)";

DEFINE_string(alignables, "", "Path to alignables spec");
DEFINE_bool(compactify, true, "Store FSTs in compact format");

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc <= 1 || argc > 3) {
    ShowUsage();
    return 2;
  }

  string out1 = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";
  string out2 = (argc > 2 && std::strcmp(argv[2], "-") != 0) ? argv[2] : "";
  if (out1.empty() && out2.empty()) {
    LOG(ERROR) << "Cannot write both outputs to stdout";
    return 2;
  }

  auto util = festus::AlignablesUtil::FromFile(FLAGS_alignables);
  if (!util) return 2;

  const auto &i2p = util->InputToPairFst();
  fst::VectorFst<ARC_TYPE(i2p)> fst(i2p);
  // TODO: Also remove forbidden factors on the output side.
  fst.SetInputSymbols(nullptr);
  fst.SetOutputSymbols(nullptr);
  if (FLAGS_compactify) {
    Compactify(fst, out1);
    std::cerr << string(80, '-') << std::endl;
  } else {
    fst::VectorFst<fst::LogArc> log_fst;
    festus::ConvertWeight(fst, &log_fst);
    log_fst.Write(out1);
  }

  fst = util->PairToOutputFst();
  fst.SetInputSymbols(nullptr);
  CHECK(fst.OutputSymbols() != nullptr);
  fst::Invert(&fst);
  fst::ArcSort(&fst, fst::OLabelCompare<ARC_TYPE(fst)>());
  if (FLAGS_compactify) {
    Compactify(fst, out2);
  } else {
    fst::VectorFst<fst::LogArc> log_fst;
    festus::ConvertWeight(fst, &log_fst);
    log_fst.Write(out2);
  }

  return 0;
}

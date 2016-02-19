// festus/alignment-diagnostics.cc
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
// Diagnostics for the Bengali pronunciation dictionary.

#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

#include <fst/compat.h>
#include <fst/determinize.h>
#include <fst/fst.h>
#include <fst/intersect.h>
#include <fst/minimize.h>
#include <fst/topsort.h>
#include <fst/vector-fst.h>

#include "festus/alignables.pb.h"
#include "festus/alignables-util.h"
#include "festus/fst-util.h"
#include "festus/label-maker.h"
#include "festus/proto-util.h"
#include "festus/string-util.h"

const char kUsage[] =
R"(Diagnostics for the Bengali pronunciation dictionary.

The dictionary must be in tab-separated value (TSV) format.
The first column (orthography) is used as the input string for alignment.
The second column (pronunciation) is used as the output string for alignment.
Any subsequent columns are ignored.

Usage:
  lexicon-diagnostics [--options...] [DICTIONARY]
)";

DEFINE_string(alignables, "", "Path to alignables spec");
DEFINE_string(string2graphemes, "", "Path to string2graphemes FST");
DEFINE_bool(filter, false, "If true, echo lines that pass checks to stdout");

namespace {

class BengaliGraphemes {
 public:
  typedef festus::AlignablesUtil::Arc Arc;

  explicit BengaliGraphemes(std::unique_ptr<fst::Fst<Arc>> string2graphemes)
      : string2graphemes_(std::move(string2graphemes)) {
  }

  fst::VectorFst<Arc> MakeInputFst(const festus::StringPiece input) {
    const unsigned char *begin =
        reinterpret_cast<const unsigned char *>(input.data());
    const unsigned char *end = begin + input.size();
    festus::CompactStringFst<Arc> string_fst;
    string_fst.SetCompactElements(begin, end);
    fst::VectorFst<Arc> graphemes;
    fst::Compose(string_fst, *string2graphemes_, &graphemes);
    fst::Project(&graphemes, fst::PROJECT_OUTPUT);
    DCHECK(graphemes.Properties(fst::kString, true));
    fst::RmEpsilon(&graphemes);
    return graphemes;
  }

 private:
  std::unique_ptr<fst::Fst<Arc>> string2graphemes_;
};

}  // namespace

int main(int argc, char *argv[]) {
  typedef festus::AlignablesUtil::Arc Arc;
  typedef fst::VectorFst<Arc> MutableLattice;

  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 2) {
    ShowUsage();
    return 2;
  }

  if (FLAGS_alignables.empty()) {
    LOG(ERROR) << "--alignables is empty; giving up.";
    return 2;
  }
  festus::AlignablesSpec spec;
  if (!festus::GetTextProtoFromFile(FLAGS_alignables.c_str(), &spec)) {
    return 2;
  }
  VLOG(1) << "BEGIN AlignablesSpec\n"
          << spec.Utf8DebugString()
          << "END AlignablesSpec";
  const auto util = festus::AlignablesUtil::New(spec);
  if (!util) {
    LOG(ERROR) << "Could not create AlignablesUtil from spec:\n"
               << spec.Utf8DebugString();
    return 2;
  }

  if (FLAGS_string2graphemes.empty()) {
    LOG(ERROR) << "--string2graphemes is empty; giving up.";
    return 2;
  }
  std::unique_ptr<fst::Fst<Arc>> string2graphemes(fst::Fst<Arc>::Read(
      FLAGS_string2graphemes));
  if (!string2graphemes) {
    return 2;
  }
  BengaliGraphemes bn_graphemes(std::move(string2graphemes));

  festus::LineReader reader;
  string in_name = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";
  if (!reader.Reset(in_name)) return 2;

  bool success = true;
  while (reader.Advance()) {
    std::vector<festus::StringPiece> fields = festus::Split(
        reader.line(), "\t");
    if (fields.size() < 2) {
      LOG(ERROR) << reader.LoggingPrefix()
                 << "Not enough fields for line: " << reader.line();
      success = false;
      continue;
    }
    MutableLattice input_fst = bn_graphemes.MakeInputFst(fields[0]);
    if (fst::kNoStateId == input_fst.Start()) {
      LOG(ERROR) << reader.LoggingPrefix()
                 << "Could not derive graphemes for line: " << reader.line();
      success = false;
      continue;
    }
    MutableLattice input_lattice = util->MakePairLatticeForInputFst(input_fst);
    if (fst::kNoStateId == input_lattice.Start()) {
      LOG(ERROR) << reader.LoggingPrefix()
                 << "Input lattice is empty for line: " << reader.line();
      success = false;
      continue;
    }
    MutableLattice output_lattice = util->MakePairLatticeForOutput(fields[1]);
    if (fst::kNoStateId == output_lattice.Start()) {
      LOG(ERROR) << reader.LoggingPrefix()
                 << "Output lattice is empty for line: " << reader.line();
      success = false;
      continue;
    }

    MutableLattice intersection, alignment_lattice;
    fst::Intersect(input_lattice, output_lattice, &intersection);
    fst::Determinize(intersection, &alignment_lattice);
    fst::AcceptorMinimize(&alignment_lattice);
    if (fst::kNoStateId == alignment_lattice.Start()) {
      LOG(ERROR) << reader.LoggingPrefix()
                 << "Alignment lattice is empty for line: " << reader.line();
      success = false;
      continue;
    }

    bool acyclic = fst::TopSort(&alignment_lattice);
    if (!acyclic) {
      LOG(WARNING) << reader.LoggingPrefix()
                   << "Alignment lattice is cyclic for line: " << reader.line();
    }

    if (FLAGS_filter) {
      std::cout << reader.line() << std::endl;
    }
  }

  if (success) {
    std::cerr << "PASS" << std::endl;
    return 0;
  } else {
    std::cerr << "FAIL" << std::endl;
    return 1;
  }
}

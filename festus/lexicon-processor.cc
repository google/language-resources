// festus/lexicon-processor.cc
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

#include "festus/lexicon-processor.h"

#include <cstring>
#include <iostream>

#include <fst/compat.h>
#include <fst/determinize.h>
#include <fst/fst.h>
#include <fst/intersect.h>
#include <fst/map.h>
#include <fst/minimize.h>
#include <fst/topsort.h>

#include "festus/alignables-util.h"
#include "festus/fst-util.h"
#include "festus/label-maker.h"
#include "festus/string-util.h"
#include "festus/runtime/fst-util.h"

DEFINE_string(alignables, "", "Path to alignables spec");
DEFINE_bool(filter, false, "If true, echo lines that pass checks to stdout");
DEFINE_int32(input_index, 0, "Column index of the input field");
DEFINE_int32(output_index, 1, "Column index of the output field");
DEFINE_bool(unique_alignments, false, "Whether alignments must be unique");

namespace festus {

bool LexiconProcessor::Init() {
  if (FLAGS_input_index >= 0) input_index_ = FLAGS_input_index;
  if (FLAGS_output_index >= 0) output_index_ = FLAGS_output_index;
  util_ = AlignablesUtil::FromFile(FLAGS_alignables);
  return util_ != nullptr;
}

bool LexiconProcessor::AlignmentDiagnostics(Entry *entry,
                                            const string &logging_prefix) {
  entry->fields = Split(entry->line, "\t");

  if (!MakeInputFst(entry)) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Could not create input FST for line: " << entry->line;
    return false;
  }
  if (fst::kNoStateId == entry->input_fst.Start()) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Input FST is empty for line: " << entry->line;
    return false;
  }
  entry->input_lattice = util_->MakePairLatticeForInputFst(entry->input_fst);
  if (fst::kNoStateId == entry->input_lattice.Start()) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Input lattice is empty for line: " << entry->line;
    return false;
  }

  if (!MakeOutputFst(entry)) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Could not create output FST for line: " << entry->line;
    return false;
  }
  if (fst::kNoStateId == entry->output_fst.Start()) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Output FST is empty for line: " << entry->line;
    return false;
  }
  entry->output_lattice = util_->MakePairLatticeForOutputFst(entry->output_fst);
  if (fst::kNoStateId == entry->output_lattice.Start()) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Output lattice is empty for line: " << entry->line;
    return false;
  }

  MutableLattice intersection;
  fst::Intersect(entry->input_lattice, entry->output_lattice, &intersection);
  util_->RemoveForbiddenFactors(&intersection);
  fst::Determinize(intersection, &entry->alignment_lattice);
  fst::AcceptorMinimize(&entry->alignment_lattice);
  if (fst::kNoStateId == entry->alignment_lattice.Start()) {
    LOG(ERROR) << logging_prefix << ":" << entry->line_number
               << ": Alignment lattice is empty for line: " << entry->line;
    return false;
  }

  bool acyclic = fst::TopSort(&entry->alignment_lattice);
  if (!acyclic) {
    LOG(WARNING) << logging_prefix << ":" << entry->line_number
                 << ": Alignment lattice is cyclic for line: " << entry->line;
  }
  return true;
}

int LexiconProcessor::AlignmentDiagnosticsMain(int argc, char *argv[]) {
  static const char kUsage[] =
      R"(Alignment diagnostics for an input/output lexicon.

The lexicon must be in tab-separated value (TSV) format.
The first column (orthography) is used as the input string for alignment.
The second column (pronunciation) is used as the output string for alignment.
Any other columns are ignored.

Usage:
  lexicon-diagnostics [--options...] [DICTIONARY]
)";
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 2) {
    ShowUsage();
    return 2;
  }

  if (!Init()) return 2;

  string in_name = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";
  LineReader reader;
  if (!reader.Reset(in_name)) return 2;
  string logging_prefix = in_name.empty() ? "<stdin>" : in_name;

  SymbolLabelMaker alignables_label_maker(util_->PairSymbols(), " ");

  bool success = true;
  Entry entry;
  while (reader.Advance(&entry)) {
    bool res = AlignmentDiagnostics(&entry, logging_prefix);
    if (FLAGS_unique_alignments) {
      double num_paths = CountPaths(&entry.alignment_lattice);
      if (num_paths != 1) {
        res = false;
        fst::StdVectorFst std_fst;
        fst::Map(entry.alignment_lattice, &std_fst, fst::Log64ToStdMapper());
        std::vector<string> alignments;
        NStrings(std_fst, 100, alignables_label_maker, &alignments);
        LOG(ERROR) << logging_prefix << ":" << entry.line_number
                   << ": Alignment is not unique for line: " << entry.line;
        for (size_t i = 0; i < alignments.size(); ++i) {
          LOG(ERROR) << "  " << (i + 1) << ".  " << alignments[i];
        }
      }
    }
    if (FLAGS_filter && res) {
      std::cout << entry.line;
      if (FLAGS_unique_alignments) {
        std::cout << "\t" << OneString(entry.alignment_lattice,
                                       alignables_label_maker);
      }
      std::cout << std::endl;
    }
    success &= res;
  }

  if (success) {
    std::cerr << "PASS" << std::endl;
    return 0;
  } else {
    std::cerr << "FAIL" << std::endl;
    return 1;
  }
}

}  // namespace festus

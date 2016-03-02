// festus/lexcion-processor.h
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
// Processing of TSV lexicon files that contain alignable input/output pairs.

#ifndef FESTUS_LEXICON_PROCESSOR_H__
#define FESTUS_LEXICON_PROCESSOR_H__

#include <cstddef>
#include <memory>
#include <vector>

#include <fst/compat.h>
#include <fst/vector-fst.h>

#include "festus/alignables-util.h"
#include "festus/string-util.h"

namespace festus {

class LexiconProcessor {
 public:
  typedef AlignablesUtil::Arc Arc;
  typedef fst::VectorFst<Arc> MutableLattice;

  struct Entry {
    size_t line_number;
    string line;
    std::vector<StringPiece> fields;
    MutableLattice input_fst;
    MutableLattice input_lattice;
    MutableLattice output_fst;
    MutableLattice output_lattice;
    MutableLattice alignment_lattice;
  };

  LexiconProcessor() = default;
  virtual ~LexiconProcessor() = default;

  virtual bool Init();

  bool LoadAlignablesUtil(const string &alignables_path);

  virtual bool MakeInputFst(Entry *entry) {
    entry->input_fst = util_->MakeInputFst(
        entry->fields.at(input_index_).ToString());
    return true;
  }

  virtual bool MakeOutputFst(Entry *entry) {
    entry->output_fst = util_->MakeOutputFst(
        entry->fields.at(output_index_).ToString());
    return true;
  }

  bool AlignmentDiagnostics(Entry *entry, const string &logging_prefix);

  int AlignmentDiagnosticsMain(int argc, char *argv[]);

 protected:
  std::unique_ptr<AlignablesUtil> util_;
  std::size_t input_index_ = 0;
  std::size_t output_index_ = 1;
};

}  // namespace festus

#endif  // FESTUS_LEXICON_PROCESSOR_H__

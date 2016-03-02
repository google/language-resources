// bn/lexicon-diagnostics.cc
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

#include <memory>

#include <fst/compat.h>
#include <fst/compose.h>
#include <fst/fst.h>
#include <fst/project.h>
#include <fst/rmepsilon.h>

#include "festus/lexicon-processor.h"

DEFINE_string(string2graphemes, "", "Path to string2graphemes FST");

namespace festus {

class BengaliLexiconProcessor : public LexiconProcessor {
 public:
  bool Init() override {
    if (!LexiconProcessor::Init()) return false;
    if (FLAGS_string2graphemes.empty()) {
      LOG(ERROR) << "Path to string2graphemes FST is empty";
      return false;
    }
    string2graphemes_.reset(fst::Fst<Arc>::Read(FLAGS_string2graphemes));
    return string2graphemes_ != nullptr;
  }

  bool MakeInputFst(Entry *entry) override {
    const auto &input = entry->fields[0];
    const unsigned char *begin =
        reinterpret_cast<const unsigned char *>(input.data());
    const unsigned char *end = begin + input.size();
    CompactStringFst<Arc> string_fst;
    string_fst.SetCompactElements(begin, end);
    auto *graphemes = &entry->input_fst;
    fst::Compose(string_fst, *string2graphemes_, graphemes);
    fst::Project(graphemes, fst::PROJECT_OUTPUT);
    DCHECK(graphemes->Properties(fst::kString, true));
    fst::RmEpsilon(graphemes);
    return true;
  }

 private:
  std::unique_ptr<fst::Fst<Arc>> string2graphemes_;
};

}  // namespace festus

int main(int argc, char *argv[]) {
  festus::BengaliLexiconProcessor proc;
  return proc.AlignmentDiagnosticsMain(argc, argv);
}

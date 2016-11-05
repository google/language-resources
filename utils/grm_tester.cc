// utils/grm_tester.cc
// Copyright 2016 Google, Inc.
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
// \file
// Used to run thrax grammer tests.

#include <thrax/grm-manager.h>

#include <assert.h>
#include <fstream>
#include <string>

using fst::StdArc;
using fst::SymbolTable;
using std::string;
using std::cout;

DEFINE_string(far, "", "Path to the far file.");
DEFINE_string(test_file, "", "Path to a tsv file which "
            " contains test entries. TSV format column 1 - thrax rule,"
            " column 2 - thrax input and column 3 expected thrax rewrite. ");

typedef fst::StringCompiler<StdArc> Compiler;
typedef fst::VectorFst<StdArc> Transducer;


int main(int argc, char *argv[]) {
  SET_FLAGS(argv[0], &argc, &argv, true);

  thrax::GrmManagerSpec<StdArc> grm_manager;
  std::unique_ptr<Compiler> compiler_;

  if (!grm_manager.LoadArchive(FLAGS_far)) {
    LOG(ERROR) << " Error cannot load far file.";
    return 0;
  }

  compiler_.reset(new Compiler(fst::StringTokenType::BYTE));
  compiler_.get();

  string output;
  Transducer input_fst;
  int error = 0;
  std::ifstream file(FLAGS_test_file);
  int line_no = 0;

  if (!file) {
    throw std::runtime_error(("failed reading file \"" + FLAGS_test_file + "\"").c_str());
  }

  for (std::string line; std::getline(file, line); ) {
    line_no++;

    // Line is a comment.
    if (line == "" || line.length() == 0 || line[0] == '#') {
      continue;
    }

    const auto segments = thrax::Split(line, "\t");

    if (segments.size() != 3) {
      continue;
    }

    if (!compiler_->operator()(segments[1], &input_fst)) {
      LOG(ERROR) << "Unable to parse input: " << segments[1];
      error = 1;
      continue;
    }

    if (!grm_manager.RewriteBytes(segments[0], input_fst, &output, "", "")) {
      LOG(ERROR) << "REWRITE_FAILED in line - " << line_no
                 << "\n line text: " + line
                 << "\n segment 0: " + segments[0]
                 << "\n segment 1: " + segments[1]
                 << "\n segment 2: " + segments[2];

      error = 1;
      continue;
    }

    if (output != segments[2]) {
      error = 1;
      LOG(WARNING) << "Error in line " << line_no << " expected - "
                   << segments[2] << " but actually - " << output;
    }
  }
  return error;
}



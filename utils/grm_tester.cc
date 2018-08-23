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

#include <fstream>
#include <string>
#include <vector>

#include <fst/compat.h>
#include <fst/string.h>
#include <fst/vector-fst.h>
#include <thrax/compat/utils.h>
#include <thrax/grm-manager.h>

using std::string;

DEFINE_string(far, "", "Path to the far file.");
DEFINE_string(test_file, "", "Path to a tsv file which "
            " contains test entries. TSV format column 1 - thrax rule,"
            " column 2 - thrax input and column 3 expected thrax rewrite. ");

int main(int argc, char *argv[]) {
  using fst::StdArc;
  typedef fst::StringCompiler<StdArc> Compiler;
  typedef fst::VectorFst<StdArc> Transducer;

  SET_FLAGS(argv[0], &argc, &argv, true);

  thrax::GrmManagerSpec<StdArc> grm_manager;
  if (!grm_manager.LoadArchive(FLAGS_far)) {
    LOG(ERROR) << "Cannot load far file: " << FLAGS_far;
    return 2;
  }

  std::ifstream file(FLAGS_test_file);
  if (!file) {
    LOG(ERROR) << "Failed to open test file: " << FLAGS_test_file;
    return 2;
  }

  Compiler compiler_(fst::StringTokenType::BYTE);
  string output;
  Transducer input_fst;
  int error = 0;
  int line_no = 0;

  for (string line; std::getline(file, line); ) {
    line_no++;

    // Line is a comment.
    if (line.empty() || line[0] == '#') {
      continue;
    }

    vector<string> segments = thrax::Split(line, "\t");

    if (segments.size() != 3 && segments.size() != 2) {
      error = 1;
      LOG(ERROR) << "Line no: " << line << " malformed";
      continue;
    }

    string rule = segments[0];
    string input = segments[1];

    if (segments.size() == 2) {
      if (!compiler_(input, &input_fst)) {
        error = 1;
        LOG(ERROR) << "Unable to parse input: " << input;
        continue;
      }

      if (grm_manager.RewriteBytes(rule, input_fst, &output, "", "")) {
        error = 1;
        LOG(WARNING) << "Expected rewrite to fail but succeeded in line - " << line_no
                   << "\n line text: " + line
                   << "\n Rule : " + rule
                   << "\n Input : " + input;
        continue;
      }
    } else {
      string expected = segments[2];

      if (!compiler_(input, &input_fst)) {
        error = 1;
        LOG(ERROR) << "Unable to parse input: " << input;
        continue;
      }

      if (!grm_manager.RewriteBytes(rule, input_fst, &output, "", "")) {
        error = 1;
        LOG(WARNING) << "REWRITE_FAILED in line - " << line_no
                   << "\n line text: " + line
                   << "\n Rule : " + rule
                   << "\n Input : " + input
                   << "\n Expected : " + expected;
        continue;
      }

      if (output != expected) {
        error = 1;
        LOG(WARNING) << "Error in line " << line_no
                     << "\n line text: " + line
                     << "\n Rule : " + rule
                     << "\n Input : " + input
                     << "\n expected : " + expected
                     << "\n actually : " << output;

      }
    }
  }
  return error;
}

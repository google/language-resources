// utils/sh-utter.cc
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
// Normalize utterances with Sparrowhawk.
//
// Reads a sentences from stdin, normalizes it, and prints it to stdout.

#include <iostream>

#include <fst/compat.h>
#include <sparrowhawk/normalizer.h>

DEFINE_string(prefix, "", "Path prefix for Sparrowhawk configuration file");
DEFINE_string(config, "", "Filename of Sparrowhawk configuration file");

int main(int argc, char *argv[]) {
  SET_FLAGS(argv[0], &argc, &argv, true);

  speech::sparrowhawk::Normalizer normalizer;
  if (!normalizer.Setup(FLAGS_config, FLAGS_prefix)) return 2;

  bool ok = true;
  string normalized;
  for (string line; std::getline(std::cin, line); ) {
    if (normalizer.Normalize(line, &normalized)) {
      std::cout << normalized << std::endl;
    } else {
      ok = false;
      LOG(ERROR) << "Could not normalize line: " << line;
    }
  }

  return ok ? 0 : 1;
}

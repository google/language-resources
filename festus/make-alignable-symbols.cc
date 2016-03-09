// festus/make-alignable-symbols.cc
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
// Prints the pair symbol table for a given alignables spec.

#include <iostream>

#include <fst/compat.h>

#include "festus/alignables-util.h"

const char kUsage[] =
    R"(Prints the pair symbol table for a given alignables spec.

Usage:
  make-alignable-symbols --alignables=spec.txt pair.syms
)";

DEFINE_string(alignables, "", "Path to alignables spec");

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 2) {
    ShowUsage();
    return 2;
  }

  string out = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";

  auto util = festus::AlignablesUtil::FromFile(FLAGS_alignables);
  if (!util) return 2;

  auto *symbols = util->PairSymbols();
  CHECK(symbols != nullptr);
  if (out.empty()) {
    CHECK(symbols->WriteText(std::cout));
  } else {
    CHECK(symbols->WriteText(out));
  }

  return 0;
}

// festus/tapes-test.cc
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
// Copyright 2018 Google LLC
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Unit test for classes that represent FST tapes.

#include "festus/tapes.h"

#include <cmath>
#include <limits>

#include <fst/vector-fst.h>
#include <gtest/gtest.h>

#include "festus/iterator.h"

namespace {

using festus::NeitherTape;
using festus::InputTape;
using festus::OutputTape;
using festus::BothTapes;

template <class Tape>
void TestInputOutput() {
  fst::StdVectorFst fst, ofst;
  festus::ArcSort<Tape>(&fst);
  festus::EpsNormalize<Tape>(fst, &ofst);
  festus::Project<Tape>(&fst);
  festus::SetSymbols<Tape>(&ofst, festus::GetSymbols<Tape>(fst));
  for (const auto state : festus::States(fst)) {
    for (const auto &arc : festus::Arcs(fst, state)) {
      CHECK_NE(festus::GetLabel<Tape>(arc), fst::kNoLabel);
    }
  }
}

TEST(TapesTest, InputTape) {
  TestInputOutput<InputTape>();
  TestInputOutput<OutputTape>();
}

template <class Tape>
void TestSortedMatcher() {
  fst::StdVectorFst fst;
  auto matcher = festus::SortedMatcher<Tape>(fst);
}

TEST(TapesTest, SortedMatcher) {
  TestSortedMatcher<NeitherTape>();
  TestSortedMatcher<InputTape>();
  TestSortedMatcher<OutputTape>();
}

TEST(TapesTest, Symbols) {
  using festus::GetSymbols;
  using festus::SetSymbols;

  fst::SymbolTable isymbols("input");
  isymbols.AddSymbol("foo");
  fst::SymbolTable osymbols("output");
  osymbols.AddSymbol("bar");

  fst::StdVectorFst fst1;
  SetSymbols<InputTape>(&fst1, &isymbols);
  SetSymbols<OutputTape>(&fst1, &osymbols);
  EXPECT_TRUE(fst::CompatSymbols(&isymbols, GetSymbols<InputTape>(fst1)));
  EXPECT_TRUE(fst::CompatSymbols(&osymbols, GetSymbols<OutputTape>(fst1)));

  fst::StdVectorFst fst2;
  SetSymbols<BothTapes>(&fst2, GetSymbols<BothTapes>(fst1));
  EXPECT_TRUE(fst::CompatSymbols(&isymbols, GetSymbols<InputTape>(fst2)));
  EXPECT_TRUE(fst::CompatSymbols(&osymbols, GetSymbols<OutputTape>(fst2)));
}

}  // namespace

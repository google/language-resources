// festus/iterator-test.cc
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
// Copyright 2015 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Unit test for iteration façades.

#include "festus/iterator.h"

#include <vector>

#include <fst/vector-fst.h>
#include <gtest/gtest.h>

namespace {

TEST(IteratorTest, StateIteration) {
  fst::StdVectorFst fst;
  for (int i = 0; i < 5; ++i) {
    fst.AddState();
  }
  auto states = festus::States(fst);
  auto iter = states.begin();
  auto end = states.end();
  EXPECT_NE(iter, end);
  EXPECT_EQ(0, *iter++);
  EXPECT_EQ(1, *iter);
  ++iter;
  EXPECT_EQ(2, *iter);
  iter++;
  EXPECT_EQ(3, *iter);
  EXPECT_EQ(3, *iter++);
  EXPECT_EQ(4, *iter);
  EXPECT_NE(iter, end);
  ++iter;
  EXPECT_EQ(iter, end);
  iter++;
  EXPECT_EQ(iter, end);
  ++iter++;
  EXPECT_EQ(iter, end);
}

TEST(IteratorTest, BeginEnd) {
  fst::StdVectorFst fst;
  for (int i = 0; i < 10; ++i) {
    fst.AddState();
    for (int j = 0; j < 10; ++j) {
      if (i == j) continue;
      fst.AddArc(i, fst::StdArc(0, 0, fst::StdArc::Weight::One(), j));
    }
  }
  auto states = festus::States(fst);
  std::vector<fst::StdArc::StateId> state_vec(states.begin(), states.end());
  EXPECT_EQ(10, state_vec.size());
  for (auto s : states) {
    auto arcs = festus::Arcs(fst, s);
    std::vector<fst::StdArc> arc_vec(arcs.begin(), arcs.end());
    EXPECT_EQ(9, arc_vec.size());
  }
}

TEST(IteratorTest, RangeBasedForLoops) {
  fst::StdVectorFst fst;
  for (int i = 0; i < 10; ++i) {
    fst.AddState();
    fst.AddArc(i, fst::StdArc(0, 0, fst::StdArc::Weight::One(), (i + 1) % 10));
  }
  int num_states = 0;
  int num_arcs = 0;
  for (auto s : festus::States(fst)) {
    EXPECT_EQ(num_states++, s);
    for (const auto &arc : festus::Arcs(fst, s)) {
      ++num_arcs;
      EXPECT_EQ(num_states % 10, arc.nextstate);
    }
  }
  EXPECT_EQ(10, num_states);
  EXPECT_EQ(10, num_arcs);
}

}  // namespace

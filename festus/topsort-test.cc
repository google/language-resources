// festus/topsort-test.cc
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
// Unit test for generalized topological sorting of FSTs.

#include "festus/topsort.h"

#include <fst/fstlib.h>
#include <gtest/gtest.h>

#include "festus/iterator.h"

namespace {

// Clears all property bits of an FST.
// Checks that no cyclicity properties are stored.
void ClearProperties(fst::StdVectorFst *fst) {
  fst->SetProperties(0, fst::kFstProperties);
  EXPECT_FALSE(fst->Properties(fst::kCyclic, false));
  EXPECT_FALSE(fst->Properties(fst::kAcyclic, false));
  EXPECT_FALSE(fst->Properties(fst::kInitialCyclic, false));
  EXPECT_FALSE(fst->Properties(fst::kInitialAcyclic, false));
  EXPECT_FALSE(fst->Properties(fst::kTopSorted, false));
  EXPECT_FALSE(fst->Properties(fst::kNotTopSorted, false));
}

// Checks that the FST is in generalized topologically order.
void TestInGeneralizedTopologicalOrder(const fst::StdVectorFst &fst) {
  for (auto s : festus::States(fst)) {
    for (const auto &arc : festus::Arcs(fst, s)) {
      EXPECT_LE(s, arc.nextstate);
    }
  }
}

TEST(TopSortTest, Loopy) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    for (int t = 0; t <= s; ++t) {
      fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), t));
    }
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  TestInGeneralizedTopologicalOrder(fst);
}

TEST(TopSortTest, LoopyInitialAcyclic) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    for (int t = 1; t <= s; ++t) {
      fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), t));
    }
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  TestInGeneralizedTopologicalOrder(fst);
}

TEST(TopSortTest, LoopyInitialAcyclic2) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    if (s != 0) {
      fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), s));
    }
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));
}

TEST(TopSortTest, Acyclic) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    for (int t = 0; t < s; ++t) {
      fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), t));
    }
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kTopSorted, true));
}

TEST(TopSortTest, Cyclic) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), s));
    fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), (s + 1) % kNumStates));
    fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), (s * 2) % kNumStates));
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_FALSE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));
}

TEST(TopSortTest, CyclicInitialAcyclic) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
    if (s != 0) {
      fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), s));
    }
    int t = (s + 1) % kNumStates;
    if (0 == t) t = 1;
    fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), t));
    t = (s * 2) % kNumStates;
    if (0 == t) t = 1;
    fst.AddArc(s, fst::StdArc(1, 1, Weight::One(), t));
  }
  fst.SetStart(0);

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));

  ClearProperties(&fst);
  EXPECT_FALSE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kCyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kNotTopSorted, true));
}

TEST(TopSortTest, Empty) {
  static const int kNumStates = 100;

  fst::StdVectorFst fst;
  typedef fst::StdVectorFst::Weight Weight;
  for (int s = 0; s < kNumStates; ++s) {
    fst.AddState();
    fst.SetFinal(s, Weight::One());
  }

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(kNumStates, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kTopSorted, true));
}

TEST(TopSortTest, Trivial) {
  fst::StdVectorFst fst;
  fst.AddState();
  fst.SetStart(0);

  EXPECT_EQ(1, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kTopSorted, true));

  ClearProperties(&fst);
  EXPECT_TRUE(festus::LoopyTopSort(&fst));

  EXPECT_EQ(1, fst.NumStates());
  EXPECT_TRUE(fst.Properties(fst::kAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kInitialAcyclic, true));
  EXPECT_TRUE(fst.Properties(fst::kTopSorted, true));
}

}  // namespace

// festus/term-semiring-test.cc
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
// Unit test for term semiring.

#include "festus/term-semiring.h"

#include <cstddef>
#include <sstream>
#include <type_traits>

#include <fst/compat.h>
#include <fst/weight.h>
#include <gtest/gtest.h>

#include "festus/weight.h"
#include "festus/weight-test-lib.h"

namespace {

template <class Props>
using BasicTermWeightTpl = festus::ValueWeightSingleton<
  festus::BasicTermSemiring<Props>,
  festus::DefaultStaticInstance<festus::BasicTermSemiring<Props>>>;

using BasicTermWeight =
    BasicTermWeightTpl<festus::FreeSemiringProperties>;

using BasicCommutativeTermWeight =
    BasicTermWeightTpl<festus::CommutativeRingProperties>;

typedef festus::TermSemiring<festus::SynchronizedMemo> FreeSemiring;

typedef festus::ValueWeightSingleton<FreeSemiring> StatefulTermWeight;

TEST(TermSemiringTest, TypeTraits) {
  EXPECT_TRUE(std::is_pod<BasicTermWeight>::value);
  EXPECT_TRUE(std::is_pod<BasicCommutativeTermWeight>::value);
  EXPECT_TRUE(std::is_pod<StatefulTermWeight>::value);
  EXPECT_EQ(8, sizeof(BasicTermWeight));
  EXPECT_EQ(8, sizeof(BasicCommutativeTermWeight));
  EXPECT_EQ(8, sizeof(StatefulTermWeight));
}

TEST(TermSemiringTest, BasicWeights) {
  festus::TestBasicWeights<BasicTermWeight>();
  festus::TestBasicWeights<BasicCommutativeTermWeight>();
  festus::TestBasicWeights<StatefulTermWeight>();
}

template <class W>
void TestTermSemiringIdentities() {
  const W w = W::From(7);
  festus::TestIdentities(W::Zero());
  festus::TestIdentities(W::One());
  festus::TestIdentities(Plus(w, w));
  festus::TestIdentities(Times(w, w));
  festus::TestIdentities(Star(w));
}

TEST(TermSemiringTest, Identities) {
  TestTermSemiringIdentities<BasicTermWeight>();
  TestTermSemiringIdentities<BasicCommutativeTermWeight>();
  TestTermSemiringIdentities<StatefulTermWeight>();
}

TEST(TermSemiringTest, Printing) {
  {
    std::ostringstream strm;
    strm << BasicTermWeight::NoWeight();
    EXPECT_EQ("Error(NoWeight)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << BasicTermWeight::Zero();
    EXPECT_EQ("Zero", strm.str());
  }
  {
    std::ostringstream strm;
    strm << BasicTermWeight::One();
    EXPECT_EQ("One", strm.str());
  }
  {
    std::ostringstream strm;
    strm << BasicTermWeight::From(0);
    EXPECT_EQ("Leaf(0)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << BasicTermWeight::From(BasicTermWeight::SemiringType::LeafLimit());
    EXPECT_EQ("Error(leaf value too large)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Plus(BasicTermWeight::From(2), BasicTermWeight::From(1));
    EXPECT_EQ("Plus(Leaf(1), Leaf(2))", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Minus(BasicTermWeight::From(2), BasicTermWeight::From(1));
    EXPECT_EQ("Error(undefined Minus)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Times(BasicTermWeight::From(2), BasicTermWeight::From(1));
    EXPECT_EQ("Times(Leaf(2), Leaf(1))", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Times(BasicCommutativeTermWeight::From(2),
                  BasicCommutativeTermWeight::From(1));
    EXPECT_EQ("Times(Leaf(1), Leaf(2))", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Divide(BasicCommutativeTermWeight::One(),
                   BasicCommutativeTermWeight::One());
    EXPECT_EQ("Error(undefined Divide)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Divide(BasicTermWeight::One(),
                   BasicTermWeight::One(),
                   fst::DIVIDE_RIGHT);
    EXPECT_EQ("Error(undefined Reciprocal)", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Star(BasicTermWeight::From(1));
    EXPECT_EQ("Star(Leaf(1))", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Times(BasicTermWeight::From(1), Star(BasicTermWeight::From(1)));
    EXPECT_EQ("KleenePlus(Leaf(1))", strm.str());
  }
  {
    std::ostringstream strm;
    strm << Star(BasicCommutativeTermWeight::One());
    EXPECT_EQ("Error(undefined Star)", strm.str());
  }
  {
    std::ostringstream strm;
    uint64 error_out_of_range = 0xFF30;
    strm << BasicTermWeight(error_out_of_range);
    EXPECT_EQ("Error(unspecified error)", strm.str());
  }
  {
    std::ostringstream strm;
    uint64 packed_payload_not_in_memo = 0xFF04;
    strm << BasicTermWeight(packed_payload_not_in_memo);
    EXPECT_EQ("Star(Error(Unpack failed))", strm.str());
  }
  {
    std::ostringstream strm;
    uint64 big_leaf = 1ULL << 27;
    strm << Plus(BasicTermWeight::From(big_leaf),
                 BasicTermWeight::From(big_leaf));
    EXPECT_EQ("Error(Pack failed)", strm.str());
  }
}

TEST(TermSemiringTest, NoMemoization) {
  typedef BasicTermWeight Weight;

  constexpr Weight leaf61 = Weight::From(1ULL << 60);
  EXPECT_FALSE(leaf61.Member());

  constexpr Weight leaf60 = Weight::From((1ULL << 60) - 1);
  EXPECT_TRUE(leaf60.Member());
  EXPECT_FALSE(Plus(leaf60, leaf60).Member());
  EXPECT_FALSE(Times(leaf60, leaf60).Member());

  constexpr Weight leaf31 = Weight::From((1ULL << 31) - 1);
  EXPECT_TRUE(leaf31.Member());
  EXPECT_FALSE(Plus(leaf31, leaf31).Member());
  EXPECT_TRUE(Times(leaf31, leaf31).Member());

  constexpr Weight leaf25 = Weight::From((1ULL << 25) - 1);
  EXPECT_TRUE(leaf25.Member());
  EXPECT_TRUE(Plus(leaf25, leaf25).Member());
  EXPECT_TRUE(Times(leaf25, leaf25).Member());

  const Weight half29 = Plus(Weight::From(255), Weight::From(255));
  const Weight half30 = Plus(Weight::From(255), Weight::From(511));
  EXPECT_TRUE(half29.Member());
  EXPECT_TRUE(half30.Member());
  const Weight full = Plus(half29, half30);
  EXPECT_TRUE(full.Member());

  EXPECT_TRUE(Plus(half29, Star(Star(Star(Star(Star(Weight::One()))))))
              .Member());
  EXPECT_TRUE(Plus(half29, Star(Star(Star(Star(Weight::From(63)))))).Member());

  // This tests private implementation details:
  EXPECT_TRUE(half29.Value() & 0x4);
  EXPECT_LT(half29.Value(), 1ULL << 29);
  EXPECT_TRUE(half30.Value() & 0x4);
  EXPECT_LT(half30.Value(), 1ULL << 30);
  EXPECT_FALSE(full.Value() & 0x4);
  EXPECT_GE(full.Value(), 1ULL << 30);
}

TEST(TermSemiringTest, Memoization) {
  typedef StatefulTermWeight Weight;
  EXPECT_EQ(0, Weight::Semiring().MemoSize());

  const Weight leaf25 = Weight::From((1ULL << 25) - 1);
  EXPECT_TRUE(leaf25.Member());
  EXPECT_EQ(0, Weight::Semiring().MemoSize());
  {
    std::ostringstream strm;
    strm << leaf25;
    EXPECT_EQ("Leaf(33554431)", strm.str());
  }

  const Weight leaf25_2 = Plus(leaf25, leaf25);
  EXPECT_TRUE(leaf25_2.Member());
  EXPECT_FALSE(leaf25_2.Value() & 0x4);  // private implementation detail
  EXPECT_EQ(0, Weight::Semiring().MemoSize());
  {
    std::ostringstream strm;
    strm << leaf25_2;
    EXPECT_EQ("Plus(Leaf(33554431), Leaf(33554431))", strm.str());
  }

  const Weight leaf25_4 = Plus(leaf25_2, leaf25_2);
  EXPECT_TRUE(leaf25_4.Member());
  EXPECT_TRUE(leaf25_4.Value() & 0x4);  // private implementation detail
  EXPECT_EQ(1, Weight::Semiring().MemoSize());
  {
    std::ostringstream strm;
    strm << leaf25_4;
    EXPECT_EQ("Plus(Plus(Leaf(33554431), Leaf(33554431)),"
                  " Plus(Leaf(33554431), Leaf(33554431)))",
              strm.str());
  }

  const Weight leaf25_8 = Plus(leaf25_4, leaf25_4);
  EXPECT_TRUE(leaf25_8.Member());
  EXPECT_TRUE(leaf25_8.Value() & 0x4);  // private implementation detail
  EXPECT_EQ(1, Weight::Semiring().MemoSize());
  {
    std::ostringstream strm;
    strm << leaf25_8;
    EXPECT_EQ("Plus(Plus(Plus(Leaf(33554431), Leaf(33554431)),"
                       " Plus(Leaf(33554431), Leaf(33554431))),"
                  " Plus(Plus(Leaf(33554431), Leaf(33554431)),"
                       " Plus(Leaf(33554431), Leaf(33554431))))",
              strm.str());
  }

  // Memo indices < 4096 can be stored compactly as children of Plus or Times
  // expressions, which means less memoization is needed. In the following
  // example the number of memoized values is half of the tree height.
  Weight w = leaf25_8;
  for (std::size_t n = 1; n < 2048; ++n) {
    EXPECT_EQ(n, Weight::Semiring().MemoSize());
    w = Plus(w, w);
    EXPECT_TRUE(w.Member());
    EXPECT_FALSE(w.Value() & 0x4);  // private implementation detail
    EXPECT_EQ(n, Weight::Semiring().MemoSize());
    w = Plus(w, w);
    EXPECT_TRUE(w.Member());
    EXPECT_TRUE(w.Value() & 0x4);   // private implementation detail
  }
  EXPECT_EQ(2048, Weight::Semiring().MemoSize());
  w = Plus(w, w);
  EXPECT_TRUE(w.Member());
  EXPECT_FALSE(w.Value() & 0x4);   // private implementation detail
  w = Plus(w, w);
  EXPECT_TRUE(w.Member());
  EXPECT_FALSE(w.Value() & 0x4);   // private implementation detail
  EXPECT_EQ(2049, Weight::Semiring().MemoSize());
  w = Plus(w, w);
  EXPECT_TRUE(w.Member());
  EXPECT_FALSE(w.Value() & 0x4);   // private implementation detail
  EXPECT_EQ(2050, Weight::Semiring().MemoSize());
}

}  // namespace

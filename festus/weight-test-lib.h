// festus/weight-test-lib.h
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
// Generic tests for OpenFst weight classes.

#include <gtest/gtest.h>

namespace festus {

template <class T>
void ExpectNe(const T &x, const T &y) {
  EXPECT_FALSE(x == y);
  EXPECT_FALSE(y == x);
  EXPECT_TRUE(x != y);
  EXPECT_TRUE(y != x);
}

template <class T>
void ExpectEq(const T &x, const T &y) {
  EXPECT_TRUE(x == y);
  EXPECT_TRUE(y == x);
  EXPECT_FALSE(x != y);
  EXPECT_FALSE(y != x);
  EXPECT_TRUE(ApproxEqual(x, y, 0));
  EXPECT_TRUE(ApproxEqual(y, x, 0));
}

template <class T>
void ExpectApproxEq(const T &x, const T &y, float delta) {
  EXPECT_TRUE(ApproxEqual(x, y, delta));
  EXPECT_TRUE(ApproxEqual(y, x, delta));
}

template <class T>
void ExpectEqIfMember(const T &x, const T &y) {
  ASSERT_TRUE(x.Member());
  if (y.Member()) {
    ExpectEq(x, y);
  }
}

// Tests the Minus operation if it is defined for weight W.
template <class W>
auto MinusTest(const W &w) -> decltype(Minus(w, w), void()) {
  VLOG(1) << "Testing Minus for " << w;
  const W n = W::NoWeight();
  const W z = W::Zero();
  EXPECT_FALSE(Minus(n, w).Member());
  EXPECT_FALSE(Minus(w, n).Member());
  if (w.Member()) {
    ExpectEqIfMember(w, Minus(w, z));
    ExpectEqIfMember(z, Minus(w, w));
    ExpectEqIfMember(z, Plus(w, Minus(z, w)));
  }
}

template <class W>
auto MinusTest(...) -> void {
  VLOG(1) << "Minus operation not available";
}

template <class W>
void TestBasicWeights() {
  const W n = W::NoWeight();
  const W z = W::Zero();
  const W o = W::One();

  // Zero and One are equal to themselves.
  ExpectEq(z, z);
  ExpectEq(o, o);

  // Zero and One are distinct from each other.
  ExpectNe(z, o);

  // NoWeight is distinct from everything, including from itself.
  ExpectNe(n, n);
  ExpectNe(n, z);
  ExpectNe(n, o);

  // Operations involving NoWeight are undefined.
  EXPECT_FALSE(n.Member());

  EXPECT_FALSE(Plus(n, n).Member());
  EXPECT_FALSE(Plus(n, z).Member());
  EXPECT_FALSE(Plus(n, o).Member());
  EXPECT_FALSE(Plus(z, n).Member());
  EXPECT_FALSE(Plus(o, n).Member());

  MinusTest(n);
  MinusTest(z);
  MinusTest(o);

  EXPECT_FALSE(Times(n, n).Member());
  EXPECT_FALSE(Times(n, z).Member());
  EXPECT_FALSE(Times(n, o).Member());
  EXPECT_FALSE(Times(z, n).Member());
  EXPECT_FALSE(Times(o, n).Member());

  EXPECT_FALSE(Star(n).Member());
}

template <class W>
void TestIdentities(const W &w, float delta = 1e-7) {
  ASSERT_TRUE(w.Member());

  ExpectEq(w, w);

  const W x = w;
  ExpectEq(w, x);

  ExpectNe(w, W::NoWeight());

  EXPECT_TRUE((w == W::Zero()) ^ (W::Zero() != w));
  EXPECT_TRUE((w != W::Zero()) ^ (W::Zero() == w));
  EXPECT_TRUE((w == W::One())  ^ (W::One()  != w));
  EXPECT_TRUE((w != W::One())  ^ (W::One()  == w));

  ExpectEq(w, Plus(w, W::Zero()));
  ExpectEq(w, Plus(W::Zero(), w));

  MinusTest(w);

  ExpectEq(w, Times(w, W::One()));
  ExpectEq(w, Times(W::One(), w));

  ExpectEq(W::Zero(), Times(w, W::Zero()));
  ExpectEq(W::Zero(), Times(W::Zero(), w));

  const W s = Star(w);
  if (s.Member()) {
    ExpectApproxEq(s, Plus(W::One(), Times(w, s)), delta);
    ExpectApproxEq(s, Plus(W::One(), Times(s, w)), delta);
    ExpectApproxEq(s, Plus(Times(w, s), W::One()), delta);
    ExpectApproxEq(s, Plus(Times(s, w), W::One()), delta);
  }
}

}  // namespace festus

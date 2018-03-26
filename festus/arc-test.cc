// festus/arc-test.cc
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
// Test C++11 traits of arc and weight types.
//
// Most of this could become a compile-time test by replacing EXPECT_TRUE with
// static_assert, but then breaking the test would mean breaking the build.
//
// This file has minimal dependencies (e.g. does not depend googletest) to
// allow for separate compilation with a variety of alternative toolchains.

#include "festus/arc.h"

#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

#include <fst/tuple-weight.h>

#include "festus/real-weight.h"

typedef festus::ValueArcTpl<festus::RealWeight> RealArc;

typedef fst::TupleWeight<festus::RealWeight, 3> R3;

template <class W>
struct MovableWeight {
  MovableWeight() = default;
  MovableWeight(std::unique_ptr<W> &&v) : value(std::move(v)) {}
  std::unique_ptr<W> value;
};

typedef festus::ReferenceArcTpl<MovableWeight<R3>> R3Arc;

int main() {
  bool success = true;

#define EXPECT_TRUE(v)                                    \
  do {                                                    \
    if (!v) {                                             \
      std::cout << "Expected true for " #v << std::endl;  \
      success = false;                                    \
    }                                                     \
  } while (false)                                         \

#ifdef __GLIBCXX__
  std::cout << "GCC libstdc++ " << __GLIBCXX__ << std::endl;
#ifdef _GLIBCXX_RELEASE
  // Defined since GCC 7.
  std::cout << "GCC libstdc++ release " << _GLIBCXX_RELEASE << std::endl;
  EXPECT_TRUE(std::is_trivially_default_constructible<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_copy_constructible<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_copy_assignable<RealArc>::value);
#else
  // Non-standard type traits have been deprecated since GCC 7.
  EXPECT_TRUE(std::has_trivial_default_constructor<RealArc>::value);
  EXPECT_TRUE(std::has_trivial_copy_constructor<RealArc>::value);
  EXPECT_TRUE(std::has_trivial_copy_assign<RealArc>::value);
#endif
#endif

#ifdef _LIBCPP_VERSION
  std::cout << "Clang libc++ " << _LIBCPP_VERSION << std::endl;
  EXPECT_TRUE(std::is_trivially_default_constructible<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_copy_constructible<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_copy_assignable<RealArc>::value);

  EXPECT_TRUE(std::is_trivially_move_constructible<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_move_assignable<RealArc>::value);
  EXPECT_TRUE(std::is_trivially_copyable<RealArc>::value);
#endif

  EXPECT_TRUE(std::is_trivially_destructible<RealArc>::value);
  EXPECT_TRUE(!std::has_virtual_destructor<RealArc>::value);
  EXPECT_TRUE(std::is_trivial<RealArc>::value);
  EXPECT_TRUE(std::is_pod<RealArc>::value);

  EXPECT_TRUE(!std::is_copy_constructible<R3Arc>::value);
  EXPECT_TRUE(std::is_move_constructible<R3Arc>::value);

  auto vec = std::unique_ptr<R3>(new R3(festus::RealWeight::One()));
  R3Arc arc(0, 0, std::move(vec), 0);
  R3Arc brc = std::move(arc);
  EXPECT_TRUE((3 == brc.weight.value->Length()));
  EXPECT_TRUE((festus::RealWeight::One() == brc.weight.value->Value(0)));

  if (success) {
    std::cout << "\033[0;32mPASS\033[m" << std::endl;
    return 0;
  } else {
    std::cout << "\033[0;31mFAIL\033[m" << std::endl;
    return 1;
  }
}

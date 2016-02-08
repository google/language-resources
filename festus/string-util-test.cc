// festus/string-util-test.cc
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
// Unit test for string utilities.

#include "festus/string-util.h"

#include <vector>

#include <gtest/gtest.h>

namespace {

TEST(StringUtilTest, Split) {
  auto split = festus::Split(" Hello \t world \r\n", " \t\n\r\f\v");
  ASSERT_EQ(2, split.size());
  EXPECT_EQ("Hello", split[0]);
  EXPECT_EQ("world", split[1]);
}

}  // namespace

// Copyright 2018 Google LLC. All Rights Reserved.
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

// \file
// Unit test for alignables-util.

#include "festus/alignables-util.h"

#include <gtest/gtest.h>

namespace festus {

void EscapeSymbol(const string &in, string *out);
bool UnescapeSymbol(const string &in, string *out);

namespace {

void EscapeRoundtrip(const string &symbol) {
  string escaped;
  EscapeSymbol(symbol, &escaped);
  string unescaped;
  EXPECT_TRUE(UnescapeSymbol(escaped, &unescaped));
  EXPECT_EQ(symbol, unescaped);
}

TEST(AlignablesUtilTest, EscapeRoundtrip) {
  EscapeRoundtrip("");
  string s;
  for (char c : {' ', '\t', '_', '/'}) {
    s = c;
    for (char d : {' ', '\t', '_', '/'}) {
      s += d;
      EscapeRoundtrip(s);
    }
  }
  EscapeRoundtrip("ab_cd/x /y\tz");
}

}  // namespace
}  // namespace festus

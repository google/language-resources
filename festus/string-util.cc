// festus/string-util.cc
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

#include "festus/string-util.h"

#include <vector>

#include <google/protobuf/stubs/stringpiece.h>

namespace festus {

std::vector<StringPiece> Split(
    const StringPiece str, const StringPiece delimiters) {
  using ::google::protobuf::stringpiece_ssize_type;
  static const stringpiece_ssize_type kNpos = StringPiece::npos;
  std::vector<StringPiece> split;
  auto begin = str.find_first_not_of(delimiters);
  while (true) {
    if (kNpos == begin) {
      break;
    }
    auto end = str.find_first_of(delimiters, begin);
    if (kNpos == end) {
      split.emplace_back(str.substr(begin));
      break;
    }
    split.emplace_back(str.substr(begin, end - begin));
    begin = str.find_first_not_of(delimiters, end);
  }
  return split;
}

}  // namespace festus

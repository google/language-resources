// festus/string-util.h
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
// String-related utility functions.

#ifndef FESTUS_STRING_UTIL_H__
#define FESTUS_STRING_UTIL_H__

#include <cstddef>
#include <fstream>
#include <istream>
#include <string>
#include <vector>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/stringpiece.h>

using std::string;

namespace festus {

using ::google::protobuf::StringPiece;

std::vector<StringPiece> Split(
    const StringPiece str, const StringPiece delimiters);

template <class Iterator>
string Join(Iterator begin, Iterator end, StringPiece delimiter) {
  if (begin == end) {
    return "";
  }
  string result(begin->begin(), begin->end());
  ++begin;
  if (begin == end) {
    return result;
  }
  string::size_type length = result.size();
  const string::size_type delimiter_length = delimiter.size();
  for (Iterator iter = begin; iter != end; ++iter) {
    length += delimiter_length + iter->size();
  }
  result.reserve(length);
  for (Iterator iter = begin; iter != end; ++iter) {
    result.append(delimiter.begin(), delimiter.end());
    result.append(iter->begin(), iter->end());
  }
  return result;
}

template <class Container>
string Join(const Container &strings, StringPiece delimiter) {
  return Join(strings.begin(), strings.end(), delimiter);
}

inline bool IsStructurallyValidUTF8(const StringPiece str) {
  return ::google::protobuf::internal::IsStructurallyValidUTF8(
      str.data(), static_cast<int>(str.size()));
}

class LineReader {
 public:
  LineReader() = default;
  ~LineReader();

  bool Reset(StringPiece path);

  template <class E>
  bool Advance(E *entry) {
    while (std::getline(*instream_, entry->line)) {
      ++line_number_;
      if (entry->line.empty() || entry->line[0] == '#') {
        continue;
      }
      entry->line_number = line_number_;
      return true;
    }
    return false;
  }

 private:
  std::ifstream infile_;
  std::istream *instream_;
  std::size_t line_number_;
};

}  // namespace festus

#endif  // FESTUS_STRING_UTIL_H__

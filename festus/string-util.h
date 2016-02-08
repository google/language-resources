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

#include <vector>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/stringpiece.h>

namespace festus {

using ::google::protobuf::StringPiece;

std::vector<StringPiece> Split(
    const StringPiece str, const StringPiece delimiters);

inline bool IsStructurallyValidUTF8(const StringPiece str) {
  return ::google::protobuf::internal::IsStructurallyValidUTF8(
      str.data(), static_cast<int>(str.size()));
}

}  // namespace festus

#endif  // FESTUS_STRING_UTIL_H__

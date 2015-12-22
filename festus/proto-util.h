// festus/proto-util.h
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
// Utility functions for interacting with protobuf messages.

#ifndef FESTUS_PROTO_UTIL_H__
#define FESTUS_PROTO_UTIL_H__

#include <fst/compat.h>
#include <google/protobuf/message.h>

namespace festus {

bool GetTextProtoFromFile(const char *filename,
                          ::google::protobuf::Message *proto);

string ShortUtf8DebugString(const ::google::protobuf::Message &message);

}  // namespace festus

#endif  // FESTUS_PROTO_UTIL_H__

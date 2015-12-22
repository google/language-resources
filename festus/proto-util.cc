// festus/proto-util.cc
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

#include "festus/proto-util.h"

#include <fcntl.h>
#include <unistd.h>

#include <fst/compat.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

using ::google::protobuf::Message;
using ::google::protobuf::TextFormat;
using ::google::protobuf::io::FileInputStream;

namespace festus {

bool GetTextProtoFromFile(const char *filename, Message *proto) {
  DCHECK(filename != nullptr);
  DCHECK(proto != nullptr);
  int fd = open(filename, O_RDONLY);
  if (-1 == fd) {
    LOG(ERROR) << "Could not open file: " << filename;
    return false;
  }
  FileInputStream input(fd);
  bool status = TextFormat::Parse(&input, proto);
  if (!status) {
    LOG(ERROR) << "Could not parse file " << filename
               << " as " << proto->GetTypeName();
  }
  if (close(fd) != 0) {
    LOG(ERROR) << "Error closing file: " << filename;
    return false;
  }
  return status;
}

string ShortUtf8DebugString(const Message &message) {
  TextFormat::Printer p;
  p.SetSingleLineMode(true);
  p.SetUseUtf8StringEscaping(true);  // Surprising, but correct.
  string s;
  p.PrintToString(message, &s);
  return s;
}

}  // namespace festus

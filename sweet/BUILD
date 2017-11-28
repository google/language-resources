# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Copyright 2017 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

proto_library(
    name = "voice_data_proto",
    srcs = ["voice_data.proto"],
)

cc_proto_library(
    name = "voice_data_cc_proto",
    deps = [":voice_data_proto"],
)

py_proto_library(
    name = "voice_data_py_proto",
    srcs = ["voice_data.proto"],
    default_runtime = "@com_google_protobuf//:protobuf_python",
    protoc = "@com_google_protobuf//:protoc",
)

cc_binary(
    name = "vocoder_world",
    srcs = ["vocoder_world.cc"],
    deps = [
        ":voice_data_cc_proto",
        "@sptk//:sptk",
        "@world//:tools",
        "@world//:world",
    ],
)

py_binary(
    name = "visualize_world",
    srcs = ["visualize_world.py"],
    srcs_version = "PY2AND3",
    deps = [
        ":voice_data_py_proto",
    ],
)

# festus/BUILD
#
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
# Copyright 2015, 2016 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "float-weight-star",
    hdrs = [
        "float-weight-star.h",
        "math-util.h",
    ],
    linkopts = ["-lm"],
    deps = ["@openfst//:weight"],
)

cc_library(
    name = "real-weight",
    hdrs = ["real-weight.h"],
    deps = ["@openfst//:weight"],
)

cc_test(
    name = "real-weight-test",
    timeout = "short",
    srcs = ["real-weight-test.cc"],
    deps = [
        "@googletest//:gtest",
        "@openfst//:weight-tester",
        ":real-weight",
    ],
)

cc_test(
    name = "star-test",
    timeout = "short",
    srcs = ["star-test.cc"],
    deps = [
        "@googletest//:gtest_main",
        "@openfst//:weight",
        ":float-weight-star",
        ":real-weight",
    ],
)

cc_library(
    name = "matrix",
    hdrs = ["matrix.h"],
    deps = ["@openfst//:fst"],
)

cc_test(
    name = "matrix-test",
    timeout = "short",
    srcs = ["matrix-test.cc"],
    linkopts = ["-lm"],
    deps = [
        "@googletest//:gtest_main",
        "@openfst//:fst",
        ":float-weight-star",
        ":matrix",
        ":real-weight",
    ],
)

cc_library(
    name = "proto-util",
    srcs = ["proto-util.cc"],
    hdrs = ["proto-util.h"],
    deps = [
        "@openfst//:base",
        "@protobuf//:protobuf",
    ],
)

cc_library(
    name = "string-util",
    srcs = ["string-util.cc"],
    hdrs = ["string-util.h"],
    deps = [
        "@openfst//:base",
        "@protobuf//:protobuf_lite",
    ],
)

cc_test(
    name = "string-util-test",
    timeout = "short",
    srcs = ["string-util-test.cc"],
    deps = [
        "@googletest//:gtest_main",
        ":string-util",
    ],
)

cc_library(
    name = "label-maker",
    srcs = ["label-maker.cc"],
    hdrs = ["label-maker.h"],
    deps = [
        "@openfst//:base",
        "@openfst//:symbol-table",
        ":string-util",
    ],
)

cc_library(
    name = "fst-util",
    hdrs = ["fst-util.h"],
    deps = [
        "@openfst//:fst",
        ":label-maker",
    ],
)

cc_library(
    name = "iterator",
    hdrs = ["iterator.h"],
    deps = ["@openfst//:fst"],
)

cc_test(
    name = "iterator-test",
    timeout = "short",
    srcs = ["iterator-test.cc"],
    deps = [
        "@googletest//:gtest_main",
        ":iterator",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
)

# Requires Bazel >= 0.1.4.
load("@protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
    name = "alignables",
    srcs = ["alignables.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
)

cc_library(
    name = "alignables-util",
    srcs = ["alignables-util.cc"],
    hdrs = ["alignables-util.h"],
    deps = [
        "@openfst//:fst",
        ":alignables",
        ":label-maker",
        ":proto-util",
    ],
)

cc_library(
    name = "lexicon-processor",
    srcs = ["lexicon-processor.cc"],
    hdrs = ["lexicon-processor.h"],
    deps = [
        "@openfst//:fst",
        ":alignables-util",
        ":fst-util",
        ":string-util",
        "//festus/runtime:fst-util",
    ],
)

cc_binary(
    name = "lexicon-diagnostics",
    srcs = ["lexicon-diagnostics.cc"],
    deps = [":lexicon-processor"],
)

cc_binary(
    name = "ngramfinalize",
    srcs = ["ngramfinalize.cc"],
    deps = [
        "@openfst//:fst",
        "@openfst//:ngram",
    ],
)

cc_binary(
    name = "make-runtime-fsts",
    srcs = ["make-runtime-fsts.cc"],
    deps = [
        "@openfst//:fst",
        "@openfst//:fstscript_info",
        ":alignables-util",
        ":iterator",
        ":types",
        "//festus/runtime:compact",
        "//festus/runtime:fst-util",
    ],
)

cc_binary(
    name = "make-alignable-symbols",
    srcs = ["make-alignable-symbols.cc"],
    deps = [
        "@openfst//:fst",
        ":alignables-util",
    ],
)

cc_proto_library(
    name = "expression",
    srcs = ["expression.proto"],
    default_runtime = "@protobuf//:protobuf",
    protoc = "@protobuf//:protoc",
)

cc_library(
    name = "expression-node",
    hdrs = ["expression-node.h"],
    deps = [
        "@openfst//:weight",
        ":expression",
    ],
)

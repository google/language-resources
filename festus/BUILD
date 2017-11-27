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
# Copyright 2015, 2016, 2017 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

alias(
    name = "gtest",
    actual = "@googletest//:gtest",
)

alias(
    name = "gtest_main",
    actual = "@googletest//:gtest_main",
)

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
    name = "weight",
    hdrs = [
        "value-weight-singleton.h",
        "value-weight-static.h",
        "weight.h",
    ],
    deps = ["@openfst//:weight"],
)

cc_library(
    name = "weight-test-lib",
    hdrs = ["weight-test-lib.h"],
    deps = [":gtest"],
)

cc_library(
    name = "real-weight",
    hdrs = ["real-weight.h"],
    deps = [
        ":types",
        ":weight",
    ],
)

cc_test(
    name = "real-weight-test",
    timeout = "short",
    srcs = ["real-weight-test.cc"],
    deps = [
        ":real-weight",
        ":weight-test-lib",
        ":gtest",
        "@openfst//:weight-tester",
    ],
)

cc_library(
    name = "modular-int-semiring",
    hdrs = ["modular-int-semiring.h"],
    deps = ["@openfst//:base"],
)

cc_test(
    name = "modular-int-semiring-test",
    timeout = "short",
    srcs = ["modular-int-semiring-test.cc"],
    deps = [
        ":modular-int-semiring",
        ":weight",
        ":weight-test-lib",
        ":gtest_main",
    ],
)

cc_library(
    name = "max-times-semiring",
    hdrs = ["max-times-semiring.h"],
    deps = ["@openfst//:base"],
)

cc_test(
    name = "max-times-semiring-test",
    timeout = "short",
    srcs = ["max-times-semiring-test.cc"],
    deps = [
        ":max-times-semiring",
        ":weight",
        ":weight-test-lib",
        ":gtest_main",
    ],
)

cc_library(
    name = "quaternion-semiring",
    hdrs = ["quaternion-semiring.h"],
    deps = [
        ":weight",
        "@openfst//:base",
    ],
)

cc_test(
    name = "quaternion-semiring-test",
    timeout = "short",
    srcs = ["quaternion-semiring-test.cc"],
    deps = [
        ":quaternion-semiring",
        ":real-weight",
        ":weight-test-lib",
        ":gtest_main",
        "@openfst//:weight",
    ],
)

cc_test(
    name = "star-test",
    timeout = "short",
    srcs = ["star-test.cc"],
    deps = [
        ":float-weight-star",
        ":real-weight",
        ":gtest_main",
        "@openfst//:weight",
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
        ":float-weight-star",
        ":matrix",
        ":real-weight",
        ":gtest_main",
        "@openfst//:fst",
    ],
)

cc_library(
    name = "proto-util",
    srcs = ["proto-util.cc"],
    hdrs = ["proto-util.h"],
    deps = [
        "@openfst//:base",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "string-util",
    srcs = ["string-util.cc"],
    hdrs = ["string-util.h"],
    deps = [
        "@openfst//:base",
        "@com_google_protobuf//:protobuf_lite",
    ],
)

cc_test(
    name = "string-util-test",
    timeout = "short",
    srcs = ["string-util-test.cc"],
    deps = [
        ":string-util",
        ":gtest_main",
    ],
)

cc_library(
    name = "label-maker",
    srcs = ["label-maker.cc"],
    hdrs = ["label-maker.h"],
    deps = [
        ":string-util",
        "@openfst//:base",
        "@openfst//:symbol-table",
    ],
)

cc_library(
    name = "fst-util",
    hdrs = ["fst-util.h"],
    deps = [
        ":label-maker",
        "@openfst//:fst",
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
        ":iterator",
        ":gtest_main",
    ],
)

cc_library(
    name = "topsort",
    hdrs = ["topsort.h"],
    deps = ["@openfst//:fst"],
)

cc_test(
    name = "topsort-test",
    timeout = "short",
    srcs = ["topsort-test.cc"],
    deps = [
        ":iterator",
        ":topsort",
        ":gtest_main",
        "@openfst//:fst",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
)

cc_library(
    name = "arc",
    hdrs = ["arc.h"],
    deps = ["@openfst//:fst"],
)

cc_test(
    name = "arc-test",
    timeout = "short",
    srcs = ["arc-test.cc"],
    deps = [
        ":arc",
        ":real-weight",
        "@openfst//:fst",
    ],
)

proto_library(
    name = "alignables_proto",
    srcs = ["alignables.proto"],
)

cc_proto_library(
    name = "alignables_cc_proto",
    deps = [":alignables_proto"],
)

cc_library(
    name = "alignables-util",
    srcs = ["alignables-util.cc"],
    hdrs = ["alignables-util.h"],
    deps = [
        ":alignables_cc_proto",
        ":label-maker",
        ":proto-util",
        "@openfst//:fst",
    ],
)

cc_library(
    name = "lexicon-processor",
    srcs = ["lexicon-processor.cc"],
    hdrs = ["lexicon-processor.h"],
    deps = [
        ":alignables-util",
        ":fst-util",
        ":string-util",
        "//festus/runtime:fst-util",
        "@openfst//:fst",
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
        ":alignables-util",
        ":iterator",
        ":types",
        "//festus/runtime:compact",
        "//festus/runtime:fst-util",
        "@openfst//:fst",
        "@openfst//:fstscript_info",
    ],
)

cc_binary(
    name = "make-alignable-symbols",
    srcs = ["make-alignable-symbols.cc"],
    deps = [
        ":alignables-util",
        "@openfst//:fst",
    ],
)

proto_library(
    name = "expression_proto",
    srcs = ["expression.proto"],
)

cc_proto_library(
    name = "expression_cc_proto",
    deps = [":expression_proto"],
)

cc_library(
    name = "expression-node",
    hdrs = ["expression-node.h"],
    deps = [
        ":expression_cc_proto",
        "@openfst//:weight",
    ],
)

cc_library(
    name = "hash",
    hdrs = ["hash.h"],
    deps = ["@farmhash//:farmhash"],
)

cc_library(
    name = "term-semiring",
    hdrs = ["term-semiring.h"],
    deps = [
        ":expression_cc_proto",
        ":weight",
        "@openfst//:base",
    ],
)

cc_test(
    name = "term-semiring-test",
    timeout = "short",
    srcs = ["term-semiring-test.cc"],
    deps = [
        ":term-semiring",
        ":weight",
        ":weight-test-lib",
        ":gtest_main",
        "@openfst//:weight",
    ],
)

cc_library(
    name = "algebraic-path",
    hdrs = ["algebraic-path.h"],
    deps = [
        "@openfst//:fst",
    ],
)

cc_test(
    name = "algebraic-path-test",
    timeout = "short",
    srcs = ["algebraic-path-test.cc"],
    deps = [
        ":algebraic-path",
        ":arc",
        ":float-weight-star",
        ":max-times-semiring",
        ":modular-int-semiring",
        ":quaternion-semiring",
        ":real-weight",
        ":weight",
        ":gtest",
        "@openfst//:fst",
    ],
)

cc_binary(
    name = "fst2re",
    srcs = ["fst2re.cc"],
    deps = [
        ":algebraic-path",
        ":arc",
        ":expression_cc_proto",
        ":term-semiring",
        ":weight",
        "@openfst//:fst",
    ],
)

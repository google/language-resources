# utils/BUILD
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
# Copyright 2016, 2017 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

exports_files(["eval.sh"])

py_binary(
    name = "python_version",
    srcs = ["python_version.py"],
    srcs_version = "PY2AND3",
)

py_binary(
    name = "phonology_json_validator",
    srcs = ["phonology_json_validator.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "uniquely_decodable",
    srcs = ["uniquely_decodable.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "evaluate_pronunciation_rules",
    srcs = ["evaluate_pronunciation_rules.py"],
    srcs_version = "PY2AND3",
    deps = [
        "//utils:icu_util",
        "//utils:utf8",
    ],
)

py_binary(
    name = "eval_dictionary_simple",
    srcs = ["eval_dictionary_simple.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "eval_lex",
    srcs = [
        "edist.py",
        "eval_lex.py",
    ],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "test_icu_transform",
    srcs = ["test_icu_transform.py"],
    srcs_version = "PY2AND3",
    deps = [
        "//utils:icu_util",
        "//utils:utf8",
    ],
)

py_binary(
    name = "icu_util",
    srcs = ["icu_util.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_library(
    name = "grapheme_util",
    srcs = ["grapheme_util.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

cc_binary(
    name = "thrax_g2p",
    srcs = ["thrax_g2p.cc"],
    deps = [
        "//festus:fst-util",
        "//festus:label-maker",
        "@com_google_absl//absl/strings",
        "@openfst//:fst",
        "@thrax//:grm-manager",
    ],
)

py_binary(
    name = "merge_lexicons",
    srcs = ["merge_lexicons.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "festival_lexicon_from_tsv",
    srcs = ["festival_lexicon_from_tsv.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "festival_prompts_from_tsv",
    srcs = ["festival_prompts_from_tsv.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

cc_binary(
    name = "sh_utter",
    srcs = ["sh-utter.cc"],
    deps = [
        "@openfst//:base",
        "@sparrowhawk//:normalizer",
    ],
)

cc_binary(
    name = "grm_tester",
    srcs = ["grm_tester.cc"],
    deps = [
        "//festus:fst-util",
        "//festus:string-util",
        "@openfst//:fst",
        "@thrax//:grm-manager",
    ],
)

cc_library(
    name = "grm_tester_lib",
    testonly = 1,
    srcs = ["grm_tester.cc"],
    deps = [
        "//festus:fst-util",
        "//festus:string-util",
        "@openfst//:fst",
        "@thrax//:grm-manager",
    ],
)

cc_binary(
    name = "lm-scores",
    srcs = ["lm-scores.cc"],
    deps = [
        "//festus:label-maker",
        "//festus/runtime:fst-util",
        "@openfst//:fst",
        "@openfst//:ngram",
    ],
)

py_binary(
    name = "sample",
    srcs = ["sample.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "freq",
    srcs = ["freq.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_library(
    name = "utf8",
    srcs = ["utf8.py"],
    srcs_version = "PY2AND3",
)

# Ordinarily, one would simply use a py_test build target, but here we want to
# test consistent behavior of utf8.py for both PY2 and PY3 and under different
# environment conditions, hence a non-standard sh_test.
sh_test(
    name = "utf8_test",
    srcs = ["utf8_test.sh"],
    args = [
        "$(location utf8_test.py)",
        "$(location utf8_test_stdout.txt)",
    ],
    data = [
        "utf8.py",
        "utf8_test.py",
        "utf8_test_stdout.txt",
    ],
)

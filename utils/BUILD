# tools/BUILD
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
# Copyright 2016 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

exports_files(["eval.sh"])

py_binary(
    name = "uniquely_decodable",
    srcs = ["uniquely_decodable.py"],
    default_python_version = "PY2",
)

py_binary(
    name = "evaluate_pronunciation_rules",
    srcs = ["evaluate_pronunciation_rules.py"],
    default_python_version = "PY2",
)

py_binary(
    name = "eval_dictionary_simple",
    srcs = ["eval_dictionary_simple.py"],
    default_python_version = "PY2",
)

py_binary(
    name = "eval_lex",
    srcs = [
        "edist.py",
        "eval_lex.py",
    ],
    default_python_version = "PY2",
)

py_binary(
    name = "test_icu_transform",
    srcs = ["test_icu_transform.py"],
    default_python_version = "PY2",
)

cc_binary(
    name = "thrax_g2p",
    srcs = ["thrax_g2p.cc"],
    deps = [
        "@openfst//:fst",
        "@thrax//:grm-manager",
        "//festus:fst-util",
        "//festus:label-maker",
        "//festus:string-util",
    ],
)

py_binary(
    name = "merge_lexicons",
    srcs = ["merge_lexicons.py"],
    default_python_version = "PY2",
)

py_binary(
    name = "festival_lexicon_from_tsv",
    srcs = ["festival_lexicon_from_tsv.py"],
    default_python_version = "PY2",
)

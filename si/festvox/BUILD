# si/festvox/BUILD
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
# Author: pasindu@google.com (Pasindu De Silva)

package(default_visibility = ["//visibility:public"])

load("//festival_utils:festvox.bzl", "make_festvox_prompts")
load("//festival_utils:festvox.bzl", "json_phonology_test")

"""
Usage
    bazel test //si/festvox:phonology_test
"""

json_phonology_test(
    phonology = "ipa_phonology.json",
)

"""
Usage
    bazel build //si/festvox:make_lexicon_scm
"""

genrule(
    name = "make_lexicon_scm",
    srcs = ["//si/data:lexicon.tsv"],
    outs = ["lexicon.scm"],
    cmd = """
          $(location //festival_utils:festival_lexicon_from_tsv) < $(location //si/data:lexicon.tsv) > $@
          """,
    tools = ["//festival_utils:festival_lexicon_from_tsv"],
)

"""
Usage
    bazel build //si/festvox:make_txt_done_data
"""

make_festvox_prompts(
    lexicon = "//si/data:lexicon.tsv",
    prompts = "//si/data:prompts.tsv",
)

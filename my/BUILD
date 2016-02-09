# my/BUILD
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

sh_test(
    name = "ipa_phonemes_are_uniquely_decodable",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        cut -f 1 $(location phoneme.syms) |
        $(location //utils:uniquely_decodable)
        """,
    ],
    data = [
        "phoneme.syms",
        "//utils:uniquely_decodable",
    ],
)

genrule(
    name = "compile_burmese_grm",
    srcs = [
        "burmese.grm",
        "grapheme.syms",
        "phoneme.syms",
    ],
    outs = ["burmese.far"],
    cmd = """
        $(location @thrax//:thraxcompiler) \
          --input_grammar=$(location burmese.grm) \
          --output_far=$@
        """,
    tools = ["@thrax//:thraxcompiler"],
)

genrule(
    name = "g2p_fst",
    srcs = ["burmese.far"],
    outs = ["G2P.fst"],
    cmd = """
        $(location @openfst//:farextract) \
          --filename_prefix=$(@D)/ \
          --keys=G2P \
          --filename_suffix=.fst \
          $<
        """,
    tools = ["@openfst//:farextract"],
)

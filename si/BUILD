# si/BUILD
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
    name = "phonemes_are_uniquely_decodable",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        cut -f1 $(location phoneme.syms) |
        fgrep -v epsilon |
        $(location //utils:uniquely_decodable)
        """,
    ],
    data = [
        "phoneme.syms",
        "//utils:uniquely_decodable",
    ],
)

genrule(
    name = "compile_sinhala_grm",
    srcs = [
        "sinhala.grm",
        "grapheme.syms",
    ],
    outs = ["sinhala.far"],
    cmd = """
        $(location @thrax//:thraxcompiler) \
          --input_grammar=$(location sinhala.grm) \
          --output_far=$@
        """,
    tools = ["@thrax//:thraxcompiler"],
)

sh_test(
    name = "graphemes_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        cut -f 1 $(location graphemes_test.tsv) |
        $(location //utils:thrax_g2p) \
          --far=$(location sinhala.far) \
          --far_g2p_key=CODEPOINTS_TO_GRAPHEMES \
          --phoneme_syms=$(location grapheme.syms) |
        diff -U0 - $(location graphemes_test.tsv)
        """,
    ],
    data = [
        "sinhala.far",
        "grapheme.syms",
        "graphemes_test.tsv",
        "//utils:thrax_g2p",
    ],
)

# jv/BUILD
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
        cut -f 2 $(location phonemes.txt) |
        $(location //utils:uniquely_decodable)
        """,
    ],
    data = [
        "phonemes.txt",
        "//utils:uniquely_decodable",
    ],
)

genrule(
    name = "compile_javanese_grm",
    srcs = [
        "javanese.grm",
        "grapheme.syms",
        "phoneme.syms",
    ],
    outs = ["javanese.far"],
    cmd = """
          $(location @thrax//:thraxcompiler) \
            --input_grammar=$(location javanese.grm) \
            --output_far=$@
          """,
    tools = ["@thrax//:thraxcompiler"],
)

genrule(
    name = "make_lexicon",
    srcs = [
        "//third_party/wikimedia:jv_sentences.txt",
        "javanese.far",
        "phoneme.syms",
    ],
    outs = [
        "bad_tokens.txt",
        "unpronouncable.txt",
        "lexicon.txt",
    ],
    cmd = """
          $(location tokenize.py) \
            < $(location //third_party/wikimedia:jv_sentences.txt) \
            2> $(location bad_tokens.txt) |
          LC_ALL=C sort -u |
          $(location //utils:thrax_g2p) \
            --far=$(location javanese.far) \
            --phoneme_syms=$(location phoneme.syms) |
          awk '{print > "/dev/std" (/ERROR/ ? "err" : "out")}' \
            1> $(location lexicon.txt) \
            2> $(location unpronouncable.txt)
          """,
    tools = [
        "tokenize.py",
        "//utils:thrax_g2p",
    ],
)

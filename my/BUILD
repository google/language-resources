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

licenses(["notice"])  # Apache 2.0

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
    name = "make_custom_phoneme_syms",
    srcs = ["phonemes.txt"],
    outs = ["custom_phoneme.syms"],
    cmd = """
          awk 'BEGIN {printf "<epsilon>\\t0\\n"}
               {printf "%s\\t%d\\n", $$1, NR}' \
            $< >$@
          """,
)

genrule(
    name = "make_ipa_phoneme_syms",
    srcs = ["phonemes.txt"],
    outs = ["ipa_phoneme.syms"],
    cmd = """
          awk 'BEGIN {printf "<epsilon>\\t0\\n"}
               {printf "%s\\t%d\\n", $$2, NR}' \
            $< >$@
          """,
)

genrule(
    name = "make_xsampa_phoneme_syms",
    srcs = ["phonemes.txt"],
    outs = ["xsampa_phoneme.syms"],
    cmd = """
          awk 'BEGIN {printf "<epsilon>\\t0\\n"}
               {printf "%s\\t%d\\n", $$4, NR}' \
            $< >$@
          """,
)

# It is difficult to get rid of phoneme.syms: doing so would make compiling the
# G2P grammar much messier, since the location of phoneme.syms must be known.
# Compromise: keep phoneme.syms and ensure that it matches phonemes.txt.
sh_test(
    name = "phoneme_syms_is_current",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = ["diff -u $(location phoneme.syms) $(location ipa_phoneme.syms)"],
    data = [
        "ipa_phoneme.syms",
        "phoneme.syms",
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
    name = "make_g2p_fst",
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

genrule(
    name = "make_b2p_fsts",
    srcs = [
        "burmese.far",
        "xsampa_phoneme.syms",
    ],
    outs = [
        "B2P.fst",
        "B2P_XSAMPA.fst",
    ],
    cmd = """
        $(location @openfst//:farextract) \
          --filename_prefix=$(@D)/ \
          --keys=B2P \
          --filename_suffix=.fst \
          $(location burmese.far) &&
        $(location @openfst//:fstsymbols) \
          --osymbols=$(location xsampa_phoneme.syms) \
          $(location B2P.fst) \
          $(location B2P_XSAMPA.fst)
        """,
    tools = [
        "@openfst//:farextract",
        "@openfst//:fstsymbols",
    ],
)

sh_binary(
    name = "g2p",
    srcs = ["g2p.sh"],
    data = [
        "G2P.fst",
        "ipa_phoneme.syms",
        "//utils:thrax_g2p",
    ],
)

sh_binary(
    name = "prepare-data-for-correction",
    srcs = ["prepare-data-for-correction.sh"],
    data = [
        "G2P.fst",
        "custom_phoneme.syms",
        "split_sentences.py",
        "tokenize.py",
        "//utils:thrax_g2p",
    ],
)

sh_binary(
    name = "train-ngram-model",
    srcs = ["train-ngram-model.sh"],
    data = [
        "codepoint.syms",
        "text_to_symbols.py",
        "@openfst//:farcompilestrings",
        "@opengrm_ngram//:ngramcount",
        "@opengrm_ngram//:ngrammake",
        "@opengrm_ngram//:ngramshrink",
    ],
)

sh_binary(
    name = "detect-charset",
    srcs = ["detect-charset.sh"],
    data = [
        "models/unicode.fst",
        "models/zawgyi.fst",
        "//utils:lm-scores",
    ],
)

sh_binary(
    name = "langid-my",
    srcs = ["langid-my.sh"],
    data = [
        "bigrams.py",
        "frequent_bigrams_my.txt",
    ],
)

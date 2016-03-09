# af/BUILD
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

sh_test(
    name = "nchlt_dictionary_test",
    timeout = "moderate",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        $(location //festus:lexicon-diagnostics) \
          --alignables=$(location alignables.txt) \
          $(location //third_party/nchlt_inlang:release/dictionaries/nchlt_afr.dict)
        """,
    ],
    data = [
        "alignables.txt",
        "//festus:lexicon-diagnostics",
        "//third_party/nchlt_inlang:release/dictionaries/nchlt_afr.dict",
    ],
)

genrule(
    name = "make_injection_fsts",
    srcs = ["alignables.txt"],
    outs = [
        "bytes_to_graphones.fst",
        "phonemes_to_graphones.fst",
    ],
    cmd = """
        $(location //festus:make-runtime-fsts) \
          --alignables=$(location alignables.txt) \
          $(location bytes_to_graphones.fst) \
          $(location phonemes_to_graphones.fst)
        """,
    tools = ["//festus:make-runtime-fsts"],
)

genrule(
    name = "make_graphone_syms",
    srcs = ["alignables.txt"],
    outs = ["graphone.syms"],
    cmd = """
        $(location //festus:make-alignable-symbols) \
          --alignables=$(location alignables.txt) \
          $@
        """,
    tools = ["//festus:make-alignable-symbols"],
)

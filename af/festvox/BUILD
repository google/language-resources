# af/festvox/BUILD
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

"""
Usage
    bazel test //af/festvox:phonology_test
"""
sh_test(
    name = "phonology_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        $(location //utils:phonology_json_validator) $(location phonology.json)
        """,
    ],
    data = [
        "phonology.json",
        "//utils:phonology_json_validator",
    ],
)

genrule(
    name = "make_lexicon_scm",
    srcs = ["//af:lex_full.txt"],
    outs = ["lexicon.scm"],
    # NOTE: The sed command replaces the NCHLT X-SAMPA phoneme h\ with plain h
    # to prevent Festival/SIOD from choking on the backslash.
    cmd = """
          sed 's/h\\\\/h/g' $(location //af:lex_full.txt) |
          $(location //utils:festival_lexicon_from_tsv) > $@
          """,
    tools = ["//utils:festival_lexicon_from_tsv"],
)

"""
Usage
    bazel build //af/festvox:make_txt_done_data
"""
genrule(
    name = "make_txt_done_data",
    srcs = [
        "//af/data:prompts.tsv",
        "//af:lex_full.txt",
    ],
    outs = ["txt.done.data"],
    cmd = """
          $(location //festival_utils:prepare_prompts.py) \
            $(location //af:lex_full.txt) \
            < $(location //af/data:prompts.tsv) \
            > $@
          """,
    tools = ["//festival_utils:prepare_prompts.py"],
)

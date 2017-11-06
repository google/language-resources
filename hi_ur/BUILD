# hi_ur/BUILD
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

py_binary(
    name = "hi2ur",
    srcs = ["hi2ur.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
    # Also depends non-hermetically on PyICU.
)

sh_test(
    name = "hi2ur_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        cat $(location hi_ur_core.tsv) $(location hi_ur_other.tsv) |
        $(location :hi2ur) $(location hi-ur.txt)
        """,
    ],
    data = [
        "hi-ur.txt",
        "hi_ur_core.tsv",
        "hi_ur_other.tsv",
        ":hi2ur",
    ],
    tags = ["manual"],  # Test is not hermetic due to dependency on PyICU.
)

py_binary(
    name = "tsv2html",
    srcs = ["tsv2html.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "symbolize",
    srcs = ["symbolize.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

sh_test(
    name = "core_alignment_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        for c in 1 2; do
          $(location symbolize) 0 $$c \
            < $(location hi_ur_core.tsv) |
          $(location //festus:lexicon-diagnostics) \
            --alignables=$(location alignables.txt) \
            --input_index=1 \
            --output_index=2
        done
        """,
    ],
    data = [
        "alignables.txt",
        "hi_ur_core.tsv",
        ":symbolize",
        "//festus:lexicon-diagnostics",
    ],
)

sh_test(
    name = "extended_alignment_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        $(location symbolize) 0 1 \
          < $(location hi_ur_extended.tsv) |
        $(location //festus:lexicon-diagnostics) \
          --alignables=$(location alignables.txt) \
          --input_index=1 \
          --output_index=2
        """,
    ],
    data = [
        "alignables.txt",
        "hi_ur_extended.tsv",
        ":symbolize",
        "//festus:lexicon-diagnostics",
    ],
)

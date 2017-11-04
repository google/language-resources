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
# Copyright 2016 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

package(default_visibility = ["//visibility:public"])

sh_test(
    name = "alignment_test",
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

py_binary(
    name = "symbolize",
    srcs = ["symbolize.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

py_binary(
    name = "tsv2html",
    srcs = ["tsv2html.py"],
    srcs_version = "PY2AND3",
    deps = ["//utils:utf8"],
)

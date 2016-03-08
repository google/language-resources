# festus/runtime/BUILD
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

cc_binary(
    name = "g2p-lookup",
    srcs = ["g2p-lookup.cc"],
    deps = [
        "@openfst//:fst",
        "@openfst//:ngram",
        ":compact",
        ":g2p",
    ],
)

cc_library(
    name = "g2p",
    hdrs = ["g2p.h"],
    deps = [
        "@openfst//:fst",
        ":fst-util",
    ],
)

cc_library(
    name = "compact",
    hdrs = ["compact.h"],
    deps = ["@openfst//:fst"],
)

cc_library(
    name = "fst-util",
    hdrs = ["fst-util.h"],
    deps = ["@openfst//:fst"],
)

cc_binary(
    name = "total-weight",
    srcs = ["total-weight.cc"],
    deps = [
        "@openfst//:fst",
        "@openfst//:ngram",
        ":fst-util",
    ],
)

sh_test(
    name = "total-weight_test",
    timeout = "short",
    srcs = ["//utils:eval.sh"],
    args = [
        """
        $(location :total-weight) \
          $(location ngram_model_with_final_backoff.fst)
        """,
    ],
    data = [
        ":total-weight",
        "ngram_model_with_final_backoff.fst",
    ],
)

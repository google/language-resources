# af/sparrowhawk/BUILD
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

sh_binary(
    name = "af_sparrowhawk",
    srcs = ["af_sparrowhawk.sh"],
    data = [
        "//af/textnorm/classifier:tokenize_and_classify_thrax_compile_grm",
        "//af/textnorm/verbalizer:verbalize_thrax_compile_grm",
        "//utils:sh_utter",
        "sparrowhawk_configuration.ascii_proto",
        "tokenizer.ascii_proto",
        "verbalizer.ascii_proto",
    ],
)

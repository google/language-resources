# Copyright 2016 Google, Inc.
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

package(default_visibility = ["//visibility:public"])

load("//:thrax.bzl", "thrax_compile_grm")

thrax_compile_grm(
    name = "universal_rules",
    deps = ["universal_depot.far"],
)

thrax_compile_grm(
    name = "universal_func",
    deps = [
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "address",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "citation",
    deps = [
        "bible_books.txt",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "date",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "digit",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "lseq",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "money",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "mapper",
    deps = [
        "money.far",
        "money.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "measure",
    deps = [
        "mapper.far",
        "mapper.grm",
        "measure_abbreviations.txt",
        "measure_plural.txt",
        "measure_prefixes.txt",
        "measure_singular.txt",
        "money.far",
        "money.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "ordinal",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "time",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "range",
    deps = [
        "date.far",
        "date.grm",
        "lseq.far",
        "lseq.grm",
        "mapper.far",
        "mapper.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "sports_teams.txt",
        "time.far",
        "time.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "telephone",
    deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "salutation",
    deps = [
        "universal_rules.far",
        "universal_rules.grm",
    ],
)


thrax_compile_grm(
    name = "classify",
    deps = [
        "address.far",
        "address.grm",
        "citation.far",
        "citation.grm",
        "date.far",
        "date.grm",
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "mapper.far",
        "mapper.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "ordinal.far",
        "ordinal.grm",
        "range.far",
        "range.grm",
        "telephone.far",
        "telephone.grm",
        "time.far",
        "time.grm",
        "salutation.grm",
        "salutation.far",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "tokenize_and_classify",
    deps = [
        "address.far",
        "address.grm",
        "citation.far",
        "citation.grm",
        "classify.far",
        "classify.grm",
        "date.far",
        "date.grm",
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "mapper.far",
        "mapper.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "ordinal.far",
        "ordinal.grm",
        "range.far",
        "range.grm",
        "telephone.far",
        "telephone.grm",
        "time.far",
        "time.grm",
        "salutation.grm",
        "salutation.far",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

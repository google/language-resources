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
    name = "number_names_rules",
    deps = ["number_names_depot.far"],
)

thrax_compile_grm(
    name = "digit",
    deps = [
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "cardinal",
    deps = [
        "digit.far",
        "digit.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "date",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "ordinal",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "decimal",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "ordinal.far",
        "ordinal.grm",
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
    name = "verbatim",
    deps = [
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
        "verbatim_mappings.txt",
    ],
)

thrax_compile_grm(
    name = "electronic",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "date.far",
        "date.grm",
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
        "verbatim.far",
        "verbatim.grm",
    ],
)

thrax_compile_grm(
    name = "fraction",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "ordinal.far",
        "ordinal.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "measure",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "decimal.far",
        "decimal.grm",
        "digit.far",
        "digit.grm",
        "fraction.far",
        "fraction.grm",
        "measure_plural.txt",
        "measure_prefixes.txt",
        "measure_singular.txt",
        "number_names_rules.far",
        "number_names_rules.grm",
        "ordinal.far",
        "ordinal.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "money",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "decimal.far",
        "decimal.grm",
        "digit.far",
        "digit.grm",
        "money_country_major_plural.txt",
        "money_country_major_single.txt",
        "money_major_single.txt",
        "number_names_rules.far",
        "number_names_rules.grm",
        "ordinal.far",
        "ordinal.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
    ],
)

thrax_compile_grm(
    name = "telephone",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
        "verbatim.far",
        "verbatim.grm",
    ],
)

thrax_compile_grm(
    name = "time",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "digit.far",
        "digit.grm",
        "lseq.far",
        "lseq.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
        "verbatim.far",
        "verbatim.grm",
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
    name = "verbalize",
    deps = [
        "cardinal.far",
        "cardinal.grm",
        "date.far",
        "date.grm",
        "decimal.far",
        "decimal.grm",
        "digit.far",
        "digit.grm",
        "electronic.far",
        "electronic.grm",
        "fraction.far",
        "fraction.grm",
        "lseq.far",
        "lseq.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "number_names_rules.far",
        "number_names_rules.grm",
        "ordinal.far",
        "ordinal.grm",
        "telephone.far",
        "telephone.grm",
        "time.far",
        "time.grm",
        "salutation.far",
        "salutation.grm",
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
        "verbatim.far",
        "verbatim.grm",
    ],
)

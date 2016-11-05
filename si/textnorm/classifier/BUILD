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
load("//:thrax.bzl", "grm_regression_test")

base_path = "si/textnorm/classifier/"

[
    grm_regression_test(
        name = test_rules + "_tests",
        far_file_path = base_path,
        test_file_path = base_path,
        far_file = test_rules + ".far",
        test_file = "testdata/" + test_rules + ".tsv",
    )
for test_rules in ["address",   "connector", "date",
                            "measure", "money", "ordinal", 
                            "roman_numeral", "time", "tokenize_and_classify"]]

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

universal_deps = [
        "universal_func.far",
        "universal_func.grm",
        "universal_rules.far",
        "universal_rules.grm",
]

[ 
    thrax_compile_grm(
        name = rule_name,
        deps = universal_deps,
    )
for rule_name in ["address", "date", "money", "ordinal", "time", "sinhala"]]

thrax_compile_grm(
    name = "measure",
    deps = [
        "money.far",
        "money.grm",
    ]  + universal_deps,
)

thrax_compile_grm(
    name = "roman_numeral",
    deps = [
        "ordinal.far",
        "ordinal.grm",
    ] + universal_deps,
)

thrax_compile_grm(
    name = "connector",
    deps = [
        "date.far",
        "date.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "time.far",
        "time.grm",
    ] + universal_deps,
)

thrax_compile_grm(
    name = "classify",
    deps = [
        "address.far",
        "address.grm",
        "connector.far",
        "connector.grm",
        "date.far",
        "date.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "ordinal.far",
        "ordinal.grm",
        "roman_numeral.far",
        "roman_numeral.grm",
        "time.far",
        "time.grm",
    ] + universal_deps,
)

thrax_compile_grm(
    name = "tokenize_and_classify",
    deps = [
        "address.far",
        "address.grm",
        "classify.far",
        "classify.grm",
        "connector.far",
        "connector.grm",
        "date.far",
        "date.grm",
        "measure.far",
        "measure.grm",
        "money.far",
        "money.grm",
        "ordinal.far",
        "ordinal.grm",
        "roman_numeral.far",
        "roman_numeral.grm",
        "sinhala.far",
        "sinhala.grm",
        "time.far",
        "time.grm",
    ] + universal_deps, 
)

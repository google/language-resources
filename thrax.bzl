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

# Experimental Bazel extension for compiling Thrax grammars.
#
# This macro is very brittle and makes undesirable assumptions. Among other
# shortcomings, it will only work with grammars that use local (or relative)
# imports, and it requires all transitive dependencies to be listed in deps.

def thrax_compile_grm(name, src=None, deps=[]):
  thraxcompiler = "@thrax//:thraxcompiler"
  if not src:
    src = name + ".grm"
  native.genrule(
    name = name + "_thrax_compile_grm",
    srcs = [src] + deps,
    outs = [name + ".far"],
    tools = [thraxcompiler],
    cmd = """
          for f in $(SRCS); do
            if [ ! -f $(@D)/$$(basename $$f) ]; then
              cp $$f $(@D)
            fi
          done &&
          $(location %s) \
            --input_grammar=$$(basename $(location %s)) \
            --indir=$(@D) \
            --output_far=$@ \
            --print_rules=false
          """ % (thraxcompiler, src),
  )

# This macro is used to run thrax grammer tests.
def grm_regression_test(name, far_file_path, test_file_path, far_file, test_file):
    native.cc_test(
        name = name,
        size = "small",
        args = ["--far=" + far_file_path + far_file, "--test_file=" + test_file_path + test_file],
        data=[
              far_file,
              test_file,
        ],
        deps = [
            "//utils:grm_tester_lib"
        ],
    )

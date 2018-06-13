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

# This macro is used to convert lexicon.tsv file to
# festvox compalible lexicon.scm file.
def make_festvox_lexicon(lexicon, name="make_lexicon_scm"):
    native.genrule(
        name = name,
        srcs = [lexicon],
        outs = ["lexicon.scm"],
        cmd = """
              $(location //festival_utils:festival_lexicon_from_tsv) < $(location """ + lexicon +""") > $@
              """,
        tools = ["//festival_utils:festival_lexicon_from_tsv"],
    )

# This macro is used to build festvox prompt file.
def make_festvox_prompts(prompts, lexicon, name="festvox"):
    native.genrule(
      name = "make_" + name,
      srcs = [
          prompts,
          lexicon,
      ],
      outs = [name + "_txt.done.data"],
      cmd = """
            $(location //festival_utils:prepare_prompts.py) \
              $(location """ + lexicon + """) \
              < $(location """ + prompts + """) \
              > $@
            """,
      tools = ["//festival_utils:prepare_prompts.py"],
    )

# This macro is used to test jphonology.json files.
def json_phonology_test(phonology, name="phonology_test"):
  native.sh_test(
      name = name,
      timeout = "short",
      srcs = ["//utils:eval.sh"],
      args = [
          """
          $(location //festival_utils:phonology_json_validator) \
          $(location """ + phonology + """)
          """,
      ],
      data = [
          phonology,
          "//festival_utils:phonology_json_validator",
      ],
  )

# A simple macro that runs all the other festvox macros.
def festvox(language, phonology="phonology.json", prompts=[], lexicon=""):
  json_phonology_test(
    phonology = phonology,
  )

  if not lexicon:
    lexicon = "//%s/data:lexicon.tsv" %(language)

  if not prompts:
    prompts = ["//%s/data:prompts.tsv" %(language)]

  make_festvox_lexicon(
    lexicon = lexicon
  )

  for prompt in prompts:
    make_festvox_prompts(
        name = "festvox_" + prompt.split(":")[-1].split(".")[0],
        lexicon = lexicon,
        prompts = prompt,
    )

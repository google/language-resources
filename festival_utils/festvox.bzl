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

# This macro is used to build festvox prompt file.
def make_festvox_prompts(prompts, lexicon, name="make_txt_done_data"):
    native.genrule(
      name = name,
      srcs = [
          prompts,
          lexicon,
      ],
      outs = ["txt.done.data"],
      cmd = """
            $(location //festival_utils:prepare_prompts.py) \
              $(location """ + lexicon + """) \
              < $(location """ + prompts + """) \
              > $@
            """,
      tools = ["//festival_utils:prepare_prompts.py"],
    )



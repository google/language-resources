# Copyright 2018 Google LLC. All Rights Reserved.
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

"""Skylark helper rules for segment display tool."""

def show_segments_test(db_name):
  """Defines a simple Bazel test for the segment displaying utility.

  Args:
     db_name: Name of the supported database.
  """
  native.sh_test(
      name = "show_segments_%s_test" % db_name,
      size = "small",
      srcs = ["//utils:eval.sh"],
      args = [
        """
        $(location show_segments.par) -db=%s | wc -l |
        grep -v -w 0 > /dev/null 2>&1
        """ % db_name,
    ],
    data = ["show_segments.par"],
  )

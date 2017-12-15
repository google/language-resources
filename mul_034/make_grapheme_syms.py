# Copyright 2017 Google LLC. All Rights Reserved.
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

"""Writes OpenFst SymbolTable for unified Indic graphemes.
"""

from __future__ import unicode_literals

import sys

from mul_034 import script_util


def main(argv):
  if len(argv) == 1:
    display_file = 'display_grapheme.syms'
    grapheme_file = 'grapheme.syms'
  elif len(argv) == 3:
    display_file = argv[1]
    grapheme_file = argv[2]
  else:
    sys.stderr.write(
        'Usage: make_grapheme_syms [display_grapheme.syms grapheme.syms]\n')
    sys.exit(2)
  data = list(script_util.ReadGraphemeDataDefault())
  display_graphemes = [(display, label) for (display, _, _, label) in data]
  graphemes = [(grapheme, label) for (_, grapheme, _, label) in data]
  success = script_util.IsBijectiveMapping(display_graphemes)
  success &= script_util.IsBijectiveMapping(graphemes)
  if not success:
    sys.exit(1)
  script_util.SymbolsToFile(display_file, display_graphemes)
  script_util.SymbolsToFile(grapheme_file, graphemes)
  return


if __name__ == '__main__':
  main(sys.argv)

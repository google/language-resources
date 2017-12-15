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

"""Writes Thrax grammar fragment for converting from codepoints to graphemes.
"""

from __future__ import unicode_literals

import sys

from mul_034 import script_util
from utils import utf8


def main(argv):
  if len(argv) == 2:
    raw_script_name = argv[1]
  else:
    utf8.stderr.write('Usage: make_grm_fragment SCRIPT_NAME\n')
    sys.exit(2)

  script = script_util.GetScript(raw_script_name)
  if not script:
    sys.exit(2)
  short_script_name = script.getShortName()

  c2g = {}
  for _, grapheme, codepoints, _ in script_util.ReadGraphemeDataDefault():
    for codepoint in codepoints.split():
      assert codepoint not in c2g, codepoint
      c2g[codepoint] = grapheme

  writer = utf8.stdout
  writer.write("c = SymbolTable['mul_034/%s.syms'];\n" % short_script_name)
  writer.write("g = SymbolTable['mul_034/grapheme.syms'];\n\n")
  writer.write('codepoint_to_grapheme =\n')
  at_start = True
  for cp_symbol, _ in script_util.ScriptSymbols(script):
    c = script_util.RemovePrefix(cp_symbol, '-')
    fully_qualified_symbol = '%s:%s' % (short_script_name, c)
    grapheme = c2g.get(fully_qualified_symbol, None)
    if not grapheme:
      grapheme = c2g.get(c, None)
    if grapheme:
      if at_start:
        prefix = ' '
        at_start = False
      else:
        prefix = '|'
      writer.write('%s ("%s".c : "%s".g)\n' % (prefix, cp_symbol, grapheme))
    else:
      writer.write('#   "%s".c has no corresponding grapheme\n' % cp_symbol)
  writer.write(';\n')
  writer.flush()
  return


if __name__ == '__main__':
  main(sys.argv)

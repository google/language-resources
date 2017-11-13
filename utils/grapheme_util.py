# Copyright 2015, 2017 Google Inc. All Rights Reserved.
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

"""Functionality for working with symbolic grapheme representations.

This is intended to make it easier to work with scripts that developers may not
be able to read fluently, by providing a one-to-one transliteration between
Unicode characters and short symbolic names.
"""

from __future__ import unicode_literals

import re

from utils import utf8

STDOUT = utf8.stdout
STDERR = utf8.stderr


def WriteSymbolTable(writer, int2sym, epsilon='<epsilon>'):
  """Write a symbol table in OpenFst text format."""
  if epsilon:
    writer.write('%s\t0\n' % epsilon)
  keys = int2sym.keys()
  keys.sort()
  for k in keys:
    writer.write('%s\t%d\n' % (int2sym[k], k))
  return


def MakeSymbolToCharDict(codepoint_to_symbol):
  return dict((v, '%c' % k) for (k, v) in codepoint_to_symbol.items())


def CharToSymbol(char, cp2sym):
  cp = ord(char)
  return cp2sym.get(cp, 'U+%04X' % cp)


def StringToSymbols(token, cp2sym):
  return ' '.join(CharToSymbol(c, cp2sym) for c in token)


def SymbolsToString(symbols, sym2ch):
  s = ''
  for symbol in symbols.split():
    if symbol not in sym2ch:
      return None
    s += sym2ch[symbol]
  return s


def TransliterateInteractively(codepoint_to_symbol):
  symbol_to_char = MakeSymbolToCharDict(codepoint_to_symbol)
  for line in utf8.stdin:
    line = line.rstrip('\r\n')
    if all(ord(c) in codepoint_to_symbol for c in line):
      orig = line
      xlit = StringToSymbols(line, codepoint_to_symbol)
    else:
      orig = SymbolsToString(line, symbol_to_char)
      xlit = line
      if orig is None:
        STDERR.write('Could not parse line: %s\n' % line)
        continue
    STDOUT.write('%s\t%s\n' % (orig, xlit))
    STDOUT.flush()
  return


def TokenizeAndTransliterate(token_re, codepoint_to_symbol):
  for line in utf8.stdin:
    for token in re.findall(token_re, line):
      xlit = StringToSymbols(token, codepoint_to_symbol)
      STDOUT.write('%s\t%s\n' % (token, xlit))
    STDOUT.flush()
  return

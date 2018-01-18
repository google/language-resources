#! /usr/bin/env python

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

"""Naive transliteration tool that prints symbolic names for codepoints."""

from __future__ import unicode_literals

import glob
import io
import os.path
import sys

STDIN = io.open(0, mode='rt', encoding='utf-8', closefd=False)
STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)


def ReadSymbolTable(path, skip_epsilon=False):
  """Read a bijective symbol table in OpenFst text format."""
  symbol2label = {}
  label2symbol = {}
  with io.open(path, mode='rt', encoding='utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split()
      assert len(fields) == 2
      symbol = fields[0]
      label = int(fields[1])
      assert symbol not in symbol2label, symbol
      assert label not in label2symbol
      if skip_epsilon and label == 0:
        continue
      symbol2label[symbol] = label
      label2symbol[label] = symbol
  return symbol2label, label2symbol


def FromScript(string, cp2sym):
  """Yield symbol names for codepoints in a string."""
  for ch in string:
    cp = ord(ch)
    if cp in cp2sym:
      yield cp2sym[cp]
    elif cp == 0x9:
      yield 'TAB'
    elif cp == 32:
      yield 'SP'
    elif 33 <= cp <= 127 or 0x2013 <= cp <= 0x2019:
      yield ch
    else:
      yield '%04X' % cp
  return


def ToScript(symbol, sym2cp):
  if symbol in sym2cp:
    return '%c' % sym2cp[symbol]
  else:
    return symbol


def RemoveSuffix(string, suffix):
  if string.endswith(suffix):
    return string[:-len(suffix)]
  else:
    return string


class Symbolizer(object):

  def __init__(self, paths):
    self.codepoint2symbol = {}
    self.script2stc = {}
    for path in paths:
      sym2cp, cp2sym = ReadSymbolTable(path, skip_epsilon=True)
      for cp, sym in cp2sym.items():
        assert self.codepoint2symbol.get(cp, sym) == sym, cp
      self.codepoint2symbol.update(cp2sym)
      script = RemoveSuffix(os.path.basename(path), '.syms').lower()
      assert script not in self.script2stc
      self.script2stc[script] = sym2cp
    return

  def IsScript(self, script):
    return script.lower() in self.script2stc

  def SymbolsToString(self, script, tokens):
    sym2cp = self.script2stc[script.lower()]
    return ''.join(ToScript(sym, sym2cp) for sym in tokens)

  def StringToSymbols(self, line):
    return ' '.join(FromScript(line, self.codepoint2symbol))


def main(argv):
  paths = glob.glob(os.path.join(os.path.dirname(argv[0]), '*.syms'))
  paths.extend(argv[1:])
  symbolizer = Symbolizer(paths)

  for line in STDIN:
    line = line.rstrip('\n')
    tokens = line.split()
    if not tokens:
      continue
    maybe_script = tokens[0].lower()
    if symbolizer.IsScript(maybe_script):
      result = symbolizer.SymbolsToString(maybe_script, tokens[1:])
    else:
      result = symbolizer.StringToSymbols(line)
    STDOUT.write('%s\n' % result)
    STDOUT.flush()
  return


if __name__ == '__main__':
  main(sys.argv)

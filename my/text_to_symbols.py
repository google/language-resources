#! /usr/bin/python2 -u
# -*- coding: utf-8 -*-
#
# Copyright 2016 Google Inc. All Rights Reserved.
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

"""Convert text to symbolic codepoint sequence.
"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def ReadSymbolTable(path):
  symtab = {}
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) == 2
      label = int(fields[1])
      if label == 0:
        continue
      symtab[label] = fields[0]
  return symtab


def TextToSymbols(text, symtab):
  return ' '.join(symtab[ord(ch)] for ch in text)


def main(argv):
  if len(argv) != 2:
    STDERR.write('Usage: %s codepoint.syms\n' % argv[0])
    sys.exit(2)
  symtab = ReadSymbolTable(argv[1])
  for line in STDIN:
    line = line.rstrip('\n')
    STDOUT.write('%s\n' % TextToSymbols(line, symtab))
  return


if __name__ == '__main__':
  main(sys.argv)

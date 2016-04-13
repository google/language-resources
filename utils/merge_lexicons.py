#! /usr/bin/python2
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

"""Merges several pronunciation dictionaries.

This is a priority-union in which an entry read from a lexicon later on the
command line superseded any entry with an identical key read earlier.

Can be used to patch lexicons by merging a large base dictionary with a small
list of corrections.

"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def ReadTsvLexicon(path):
  """Reads a lexicon in TSV format. Only the first 2 columns are used."""
  lex = {}
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) >= 2
      orth, pron = fields[:2]
      if orth not in lex:
        lex[orth] = []
      lex[orth].append(pron)
  return lex


def main(argv):
  if len(argv) == 1:
    STDOUT.write('Usage: %s LEXICON...\n' % argv[0])
    sys.exit(1)
  lex = {}
  for path in argv[1:]:
    lex.update(ReadTsvLexicon(path))
  words = lex.keys()
  words.sort()
  for orth in words:
    for pron in lex[orth]:
      STDOUT.write('%s\t%s\n' % (orth, pron))
  return


if __name__ == '__main__':
  main(sys.argv)

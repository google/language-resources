# Copyright 2016, 2017 Google Inc. All Rights Reserved.
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

import sys

import utf8

STDIN = utf8.stdin
STDOUT = utf8.stdout


def ReadTsvLexicon(reader):
  """Reads a lexicon in TSV format. Only the first 2 columns are used."""
  lex = {}
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
    sys.exit(2)
  lex = {}
  for path in argv[1:]:
    if path == '-':
      lex.update(ReadTsvLexicon(STDIN))
    else:
      with utf8.open(path) as reader:
        lex.update(ReadTsvLexicon(reader))
  for orth in sorted(lex):
    for pron in lex[orth]:
      STDOUT.write('%s\t%s\n' % (orth, pron))
  return


if __name__ == '__main__':
  main(sys.argv)

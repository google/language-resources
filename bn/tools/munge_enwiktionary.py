#! /usr/bin/env python

# Copyright 2015 Google Inc. All Rights Reserved.
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

# -*- coding: utf-8 -*-
#
# Extract Bengali words/phrases with transliteration from (English-metalanguage)
# Wiktionary.



import bz2
import codecs
import re
import sys

_STDIN  = codecs.lookup('utf-8').streamreader(sys.stdin)
_STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)
_STDERR = codecs.lookup('utf-8').streamwriter(sys.stderr)

# Quickly find lines that contain "|bn|".
BN_RE = re.compile(r'.*\|bn\|')

# Find Mediawiki template substitution containing "|bn|":
TEMPLATE_RE = re.compile(r'\{\{[^}]*\|bn\|[^}]*\}\}')

# Match Bengali words:
BENG_RE = re.compile(ur'[][ !.?\u0964\u0980-\u09FF\u200c\u2019-]+$')

def NoDebug(x):
  return

def Debug(x):
  _STDOUT.write(x)

def ReadBnLines(path):
  if path.endswith('bz2'):
    reader = codecs.lookup('utf-8').streamreader(bz2.BZ2File(path, 'r'))
  else:
    reader = codecs.open(path, 'r', 'utf-8')
  for line in reader:
    if not BN_RE.match(line):
      continue
    yield line
  reader.close()

def ParseBnLine(line, debug=NoDebug):
  debug(line)
  for match in TEMPLATE_RE.finditer(line):
    debug(match.group(0)[2:-2] + '\n')
    fields = match.group(0)[2:-2].split('|')
    if len(fields) < 4 or fields[1] != 'bn':
      continue
    bn = None
    for field in fields[2:]:
      if BENG_RE.match(field):
        bn = field
        break
    if bn is None:
      continue
    bn = bn.replace('[', '').replace(']', '')
    tr = None
    for field in fields[3:]:
      if field.startswith('tr='):
        if tr is None:
          tr = field[3:]
        else:
          debug('Multiple tr= fields, skipping\n')
          tr = None
          break
    if tr is None:
      continue
    yield bn, tr
  debug('\n')

if __name__ == '__main__':
  if len(sys.argv) != 2:
    _STDERR.write('Usage: %s enwikitionary-*-pages-articles.xml.bz2\n'
                  % sys.argv[0])
    sys.exit(1)
  for line in ReadBnLines(sys.argv[1]):
    # _STDOUT.write(line)
    # continue
    for bn_tr in ParseBnLine(line, NoDebug):
      _STDOUT.write('%s\t%s\n' % bn_tr)

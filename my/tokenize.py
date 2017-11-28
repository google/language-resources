#! /usr/bin/env python
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

"""Simple tokenizer for Burmese sentences.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

TOKEN = re.compile(r'''
  (?P<native>[\u1000-\u103F\u104C-\u104F\u1050-\u108F\u109A-\u109D\u200C\uAA60-\uAA7F\uA9E0-\uA9EF\uA9FA-\uA9FE]+)
| (?P<latin>[A-Za-z]+)
| (?P<number>[\u101D\u1040-\u1049]*[\u1040-\u1049]+[\u101D\u1040-\u1049]*)
| (?P<number1>[0-9]+)
| (?P<number2>[\u1090-\u1099]+)
| (?P<number3>[\uA9F0-\uA9F9]+)
| (?P<punct>[-"&'()*,./:?\[\]\u104A\u104B\u2013\u2014\u2018\u2019\u201C\u201D\u2026]+)
| (?P<space>[\u0020\u00A0\u200B]+)
| (?P<symbol>[$+=|\u109E\u109F]+)
''', re.VERBOSE)


def GetToken(match):
  items = [(k, v) for (k, v) in match.groupdict().items() if v is not None]
  assert len(items) == 1
  return items[0]


def Tokenize(line):
  end = 0
  for match in TOKEN.finditer(line):
    if match.start() != end:
      unmatched = line[end:match.start()]
      yield None, unmatched
    kind_token = GetToken(match)
    yield kind_token
    end = match.end()
  if end < len(line):
    yield None, line[end:]
  return


def Debug():
  for line in STDIN:
    STDOUT.write('\n%s\n' % line)
    line = line.rstrip('\n')
    for kind, token in Tokenize(line):
      if kind:
        STDOUT.write('token %6s: %s\n' % (kind, token))
      else:
        STDOUT.write('Unmatched: %s\n' % repr(token))
  return


def main():
  for line in STDIN:
    line = line.rstrip('\n')
    if not line:
      STDOUT.write('\n')
    tokens = list(Tokenize(line))
    if not tokens:
      continue
    if all(kind in ('native', 'punct', 'space') for (kind, token) in tokens):
      STDOUT.write('%s\n' % ' '.join(token for (kind, token) in tokens
                                     if kind == 'native'))
    else:
      STDERR.write('%s\n' % line)
  return


if __name__ == '__main__':
  if len(sys.argv) > 1 and sys.argv[1].endswith('debug'):
    Debug()
  else:
    main()

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

"""Burmese grapheme clusters.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)

GRAPHEME_CLUSTER = re.compile(r'''
  [()\u1040-\u104D\u104F\u200B]
| (\u1004\u103A\u1039)? \u104E
| ([\u1004\u101B]\u103A\u1039)?                # kinzi etc. above
  [\u1000-\u102A\u103F\u1050-\u1055]           # main independent letter
  (\u1039[\u1000-\u102A\u103F\u1050-\u1055])*  # stacked consonant below
  [\u103A-\u103E\u200C\u200D]*                 # asat and medials
  [\u102B-\u1035\u1056-\u1059]*                # dependent vowels
  [\u1036\u1037\u1038\u103A]*                  # final diacritics
''', re.VERBOSE)


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def GraphemeClusters(text):
  end = 0
  for match in GRAPHEME_CLUSTER.finditer(text):
    if match.start() != end:
      unmatched = text[end:match.start()]
      yield False, unmatched
    yield True, match.group(0)
    end = match.end()
  if end < len(text):
    yield False, text[end:]
  return


if __name__ == '__main__':
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    STDOUT.write('Line: %s\n' % line)
    for matched, text in GraphemeClusters(line):
      if matched:
        STDOUT.write('  Grapheme cluster: %s\n' % text)
      else:
        STDOUT.write('  Unmatched: %s\n' % repr(text))

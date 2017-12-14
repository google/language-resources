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

"""Burmese grapheme clusters.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)

UNICODE_GRAPHEME_CLUSTER = re.compile(r'''
  [-()\u1041-\u104D\u104F\u200B]
| (\u1004\u103A\u1039)? \u104E
| ([\u1004\u101B\u105A]\u103A\u1039)?          # kinzi above
  [\u1000-\u102A\u103F\u1040\u1050-\u1055]     # main independent letter
  (\u1039[\u1000-\u102A\u103F\u1050-\u1055])*  # stacked consonant below
  [\u103A-\u103E\u200C\u200D]*                 # asat and medials
  [\u102B-\u1035\u1056-\u1059]*                # dependent vowels
  [\u1036\u1037\u1038\u103A]*                  # final diacritics
''', re.VERBOSE)

ZAWGYI_GRAPHEME_CLUSTER = re.compile(r'''
  [-()\u1041-\u104F\u200B]
| \u1031*                                                        # prevowel e
  [\u103B\u107E-\u1084]?                                         # medial r
  [\u1000-\u102A\u1040\u106A\u106B\u106E\u106F\u1086\u108F-\u1092\u1097]
  ( [\u102B-\u1030\u1032-\u103A\u103C-\u103F\u105A\u1060-\u1069\u106C\u106D]
  | [\u1070-\u107D\u1085\u1087-\u108E\u1093-\u1096\u200C\u200D]
  )*
''', re.VERBOSE)


class GraphemeClusterer(object):

  def __init__(self, which):
    if which.startswith('z'):
      self.pattern = ZAWGYI_GRAPHEME_CLUSTER
    else:
      self.pattern = UNICODE_GRAPHEME_CLUSTER
    return

  def GraphemeClusters(self, text):
    end = 0
    for match in self.pattern.finditer(text):
      if match.start() != end:
        unmatched = text[end:match.start()]
        yield False, unmatched
      yield True, match.group(0)
      end = match.end()
    if end < len(text):
      yield False, text[end:]
    return


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


if __name__ == '__main__':
  if len(sys.argv) != 2 or sys.argv[1].lower() not in ('unicode', 'zawgyi'):
    STDOUT.write('Usage: %s (unicode|zawgyi)\n' % sys.argv[0])
    sys.exit(2)

  clusterer = GraphemeClusterer(sys.argv[1].lower())

  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    STDOUT.write('Line\t%s\n' % line)
    for matched, text in clusterer.GraphemeClusters(line):
      STDOUT.write('%s\t%s\t%s\n' %
                   ('Cluster' if matched else 'Unmatched',
                    text,
                    ' '.join('%04X' % ord(c) for c in text)))

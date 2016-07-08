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

"""Simple pattern-based encoding classification of Burmese strings.

Reads line-by-line from stdin and writes TSV format to stdout. Each input line
should consist of Myanmar codepoints, as produced by extract_text.py. The TSV
output is of the form Label TAB Line, where Label can be one of:

* "Not Myanmar": The line contains codepoints outside of the Myanmar Unicode
  blocks.

* "Neither": The line is neither consistent with Zawgyi nor with Unicode 5.1
  encoding of Burmese.

* "Zawgyi": The line is consistent with Zawgyi font encoding of Burmese,
  but not with Unicode 5.1 encoding of Burmese.

* "Unicode": The line is consistent with Unicode 5.1 encoding of Burmese,
  but not with Zawgyi font encoding of Burmese.

* "Zawgyi|Unicode": The line is consistent with both Zawgyi and Unicode 5.1
  encoding of Burmese, but its meaning differs between those encodings.

* "Consensus": The line is consistent with both Zawgyi and Unicode 5.1
  encoding of Burmese and has the same meaning in both encodings.

This script is only useful when the underlying language is Burmese. It is
easily confused by other languages written in the Myanmar script and is not
designed to be used for language detection. Examples (all in Unicode):

* "တူ" is classified as "Consensus", but in addition to Burmese it could be a
  Mon or a Shan word.

* The Shan word "မိူင်း" is classified as "Zawgyi|Unicode".

* "တီႇသႅမ်ႇပႃႇ" is the Shan transliteration of "December", but it is classified
  as "Zawgyi".

"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

EITHER = re.compile(r'''
( [-()]
| [\u1000-\u109F]
| [\uAA60-\uAA7F]
| [\uA9E0-\uA9FF]
| [\u200B-\u200D]
)*$
''', re.VERBOSE)

CONSENSUS = re.compile(r'''
( [-()]
| [\u1000-\u1021]
# 1022 is undefined in Zawgyi
| [\u1023-\u1027]
# 1028 is undefined in Zawgyi
| [\u1029-\u1030]
# 1031 has different behavior in Zawgyi
| [\u1032]
# 1033 and 1034 have different meaning in Zawgyi
# 1035 is undefined in Zawgyi
| [\u1036-\u1038]
# 1039 through 103D have different meaning in Zawgyi
# 103E and 103F are undefined in Zawgyi
| [\u1040-\u104D]
# 104E has slightly different meaning in Zawgyi
| [\u104F]
# 1050 through 109F are either undefined or have different meaning in Zawgyi
| [\u200B-\u200D]
)*$
''', re.VERBOSE)

BURMESE_UNICODE_51 = re.compile(r'''
( [-()]
| [\u1000-\u1021]
# 1022 is not used for writing Burmese
| [\u1023-\u1027]
# 1028 is not used for writing Burmese
| [\u1029-\u1032]
# 1033 through 1035 are not used for writing Burmese
| [\u1036-\u104F]
# 1050 through 109F are not used for writing Burmese
| [\u200B-\u200D]
)*$
''', re.VERBOSE)

ZAWGYI = re.compile(r'''
( [-()]
| [\u1000-\u1021]
# 1022 is undefined in Zawgyi
| [\u1023-\u1027]
# 1028 is undefined in Zawgyi
| [\u1029-\u1034]
# 1035 is undefined in Zawgyi
| [\u1036-\u103D]
# 103E and 103F are undefined in Zawgyi
| [\u1040-\u104F]
# 1050 through 105F are undefined in Zawgyi except for 105A
| [\u105A]
| [\u1060-\u1097]
| [\u200B-\u200D]
)*$
''', re.VERBOSE)


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def ClassifyCharset(text):
  if not EITHER.match(text):
    return 'Not Myanmar'
  zawgyi = ZAWGYI.match(text)
  my_uni = BURMESE_UNICODE_51.match(text)
  if CONSENSUS.match(text):
    assert zawgyi
    assert my_uni
    return 'Consensus'
  if zawgyi:
    if my_uni:
      return 'Zawgyi|Unicode'
    else:
      return 'Zawgyi'
  elif my_uni:
    return 'Unicode'
  else:
    return 'Neither'


def main(unused_argv):
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    label = ClassifyCharset(line)
    STDOUT.write('%s\t%s\n' % (label, line))
  return


if __name__ == '__main__':
  main(sys.argv)

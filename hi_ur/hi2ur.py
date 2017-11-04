# -*- coding: utf-8 -*-
#
# Copyright 2017 Google Inc. All Rights Reserved.
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

"""Apply the Hindi-to-Urdu transliteration rules.

Reads input in TSV format from stdin. The first column must contain Hindi text
to be transliterted. The optional second column may contain diacritized Urdu
text that will be compared against the output of the transliteration
rules. The optional third column may contain undiacritized Urdu text that will
be compared against the output of the transliteration rules after optional
diacritics have been removed.

Diacritized and undiacritized transliteration output is appended to the input
fileds and written to stdout or to stderr, depending on whether
transliteration and optional checking was successful. Additional diagnostics
are written to stderr.
"""

from __future__ import unicode_literals

import sys
import unicodedata

import icu  # Debian/Ubuntu: apt-get install python-pyicu python3-icu

from utils import utf8


def DebugString(string):
  for c in string:
    yield '%s\t%04X\t%s' % (c, ord(c), unicodedata.name(c, ''))
  return


def StrEqual(expected, observed, context=None):
  """Checks if two strings are equal and provides diagnostic output if not."""
  equal = observed == expected
  if not equal:
    if context:
      utf8.stderr.write('Context: %s\n' % context)
    utf8.stderr.write('''Expected:
%s
Observed:
%s
''' % ('\n'.join(DebugString(expected)), '\n'.join(DebugString(observed))))
    utf8.stderr.flush()
  return equal


REMOVE_TASHKIL = icu.Transliterator.createInstance(
    '[\u064B-\u065F]Remove',
    icu.UTransDirection.FORWARD)


def StrEqualIgnoringTashkil(str1, str2, context=None):
  str1_without = REMOVE_TASHKIL.transliterate(str1)
  str2_without = REMOVE_TASHKIL.transliterate(str2)
  return StrEqual(str1_without, str2_without, context=context)


def main(argv):
  with utf8.open(argv[1]) as reader:
    rules = reader.read()
  hi_ur = icu.Transliterator.createFromRules(
      'hi-ur', rules, icu.UTransDirection.FORWARD)
  success = True
  for line in utf8.stdin:
    line = line.rstrip('\n')
    if not line or line.startswith('#'):
      continue
    line = unicodedata.normalize('NFC', line)
    fields = line.split('\t')
    hi = fields[0]
    ur_with_diacritics = hi_ur.transliterate(hi)
    ur_without_diacritics = REMOVE_TASHKIL.transliterate(ur_with_diacritics)
    line_success = True
    if len(fields) > 1 and fields[1]:
      line_success &= StrEqual(fields[1], ur_with_diacritics, context=hi)
    if len(fields) > 2 and fields[2]:
      if fields[1]:
        assert StrEqualIgnoringTashkil(fields[1], fields[2], context=hi)
      line_success &= StrEqual(fields[2], ur_without_diacritics, context=hi)
    success &= line_success
    fields.extend([ur_with_diacritics, ur_without_diacritics])
    utf8.Print('\t'.join(fields),
               file=utf8.stdout if line_success else utf8.stderr, flush=True)
  sys.exit(0 if success else 1)
  return


if __name__ == '__main__':
  main(sys.argv)

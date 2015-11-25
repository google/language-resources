#! /usr/bin/python
# -*- coding: utf-8 -*-
#
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

"""Utility for context-aware removal of ZWNJ and ZWJ in Bangla text.

Tested with:
$ python2 -m unittest zero_width
$ python3 -m unittest zero_width
"""

from __future__ import unicode_literals

import re
import sys
import unittest


STANDARDIZE_ZW = re.compile(r'(?<=\u09b0)[\u200c\u200d]+(?=\u09cd\u09af)')

DELETE_ZW = re.compile(r'(?<!\u09b0)[\u200c\u200d](?!\u09cd\u09af)')


def RemoveOptionalZW(text):
  """Removes all optional occurrences of ZWNJ or ZWJ from Bangla text.

  The non-printing characters U+200C (ZWNJ) and U+200D (ZWJ) are used in Bangla
  to optionally control the appearance of ligatures, except in one special
  situation: after RA and before VIRAMA+YA, the presence or absence of ZWJ
  (formerly ZWNJ) changes the visual appearance of the involved consonants in a
  meaningful way. This occurrences of ZWJ must be preserved, while all other
  occurrences are advisory and can be removed for most purposes.

  After RA and before VIRAMA+YA, this function changes ZWNJ to ZWJ and preserves
  ZWJ; and removes ZWNJ and ZWJ everywhere else.

  Args:
    text: The text from which the zero-width format controls are to be removed.

  Returns:
    The text with all non-obligatory occurrences of ZWNJ and ZWJ removed.

  """
  text = STANDARDIZE_ZW.sub('\u200D', text)
  text = DELETE_ZW.sub('', text)
  return text


class TestRemoveOptionalZW(unittest.TestCase):

  def TestRemoveOptionalZW(self):
    rally = '\u09b0\u200d\u09cd\u09af\u09be\u09b2\u09c0'
    self.assertEqual(rally, 'র‍্যালী')
    self.assertEqual(RemoveOptionalZW(rally), rally)
    for i in range(len(rally) + 1):
      for zw in ('\u200c', '\u200d', '\u200c\u200d', '\u200d\u200c'):
        text = rally[:i] + zw + rally[i:]
        self.assertEqual(RemoveOptionalZW(text), rally)


if __name__ == '__main__':
  for line in sys.stdin:
    line = line.decode('utf-8')
    line = RemoveOptionalZW(line)
    sys.stdout.write(line.encode('utf-8'))

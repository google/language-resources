#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015, 2016, 2017 Google Inc. All Rights Reserved.
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

"""Utilities for removal and insertion of ZWNJ and ZWJ in Sinhala text.

Tested with:
$ python2 -m unittest zero_width
$ python3 -m unittest zero_width
"""

from __future__ import unicode_literals

import io
import re
import unittest

SINHALA_TEXT = re.compile(r'''
( [\u0d80-\u0dff\u200c\u200d]*
  [\u0d80-\u0dff]
  [\u0d80-\u0dff\u200c\u200d]* )
''', re.VERBOSE)

ZERO_WIDTH = re.compile(r'[\u200c\u200d]+')

CONSONANT_RAKARANSAYA_ONSETS = [
    'ක',  # odds 60072:2236, prob 96.4%
    'ග',  # odds 21310:818, prob 96.3%
    'ඝ',  # odds 644:15, prob 97.6%
    'ට',  # odds 9534:592, prob 94.1%
    'ඩ',  # odds 2275:661, prob 77.5%
    'ත',  # odds 40024:1168, prob 97.2%
    'ද',  # odds 17638:713, prob 96.1%
    'ධ',  # odds 550:23, prob 95.8%
    'ඳ',  # odds 15:0, prob 94.1%
    'ප',  # odds 141170:3352, prob 97.7%
    'බ',  # odds 8854:658, prob 93.1%
    'භ',  # odds 674:30, prob 95.6%
    'ශ',  # odds 56064:535, prob 99.1%
    'ස',  # odds 849:284, prob 74.9%
    'ෆ',  # odds 1703:447, prob 79.2%
    ]

LIKELY_RAKARANSAYA = re.compile(
    r'(?<=[%s]\u0dca)(?=\u0dbb)' % ''.join(CONSONANT_RAKARANSAYA_ONSETS))


def RemoveOptionalZeroWidthInMatch(match):
  text = match.group(1)
  # standardize_zero_width
  text = ZERO_WIDTH.sub('\u200d', text)
  # ins_repaya
  text = text.replace('\u0dbb\u0dca\u200d', '\uE003')
  # ins_medials
  text = text.replace('\u0dca\u200d\u0dba', '\uE001')
  text = text.replace('\u0dca\u200d\u0dbb', '\uE002')
  # del_zwj
  text = text.replace('\u200d', '')
  # expand_repaya_and_medials
  text = text.replace('\uE001', '\u0dca\u200d\u0dba')
  text = text.replace('\uE002', '\u0dca\u200d\u0dbb')
  text = text.replace('\uE003', '\u0dbb\u0dca\u200d')
  return text


def RemoveOptionalZW(text):
  """Removes all optional occurrences of ZWNJ or ZWJ from modern Sinhala text.

  The non-printing characters U+200C (ZWNJ) and U+200D (ZWJ) are used in Sinhala
  for two main purposes: (1) to denote yansaya, rakaransaya, and repaya; and (2)
  to control the appearance of ligatures and touching glyphs. In modern Sinhala
  orthography, the use of ZWJ is obligatory as part of yansaya and
  rakaransaya. It is debatable whether repaya is obligatory, but we will treat
  it as such and not remove ZWJ (to remove ZWJ from repaya, use
  RemoveRepaya). All other uses of ZWNJ and ZWJ are optional/adivsory and will
  be removed.

  This method changes ZWNJ to ZWJ in the context of yansaya, rakaransaya, and
  repaya and preserves it; and removes ZWNJ and ZWJ everywhere else.

  Args:
    text: The text from which the zero-width format controls are to be removed.

  Returns:
    The text with all non-obligatory occurrences of ZWNJ and ZWJ removed.
  """
  return SINHALA_TEXT.sub(RemoveOptionalZeroWidthInMatch, text)


def RemoveRepaya(text):
  """Replaces all occurrences of repaya with pure ra + al-lakuna."""
  # The sequence 0DBB (ra), 0DCA (virama), 200D (ZWJ), 0DBA (ya) is potentially
  # ambiguous. The first three characters represent repaya; the last three
  # characters represent yansaya. Per Sri Lanka Standard SLS 1134 : 2011, the
  # sequence ra + yansaya does not occur, so we assume that 0DBB 0DCA 200D 0DBA
  # actually represents repaya + ya. In other words repaya is 0DBB 0DCA 200D in
  # all contexts and can safely be replaced with plain ra+virama globally.
  return text.replace('\u0dbb\u0dca\u200d', '\u0dbb\u0dca')


def ForceRakaransaya(text):
  """Inserts ZWJ heuristically to force the appearance of rakaransaya.

  One frequently finds ශ්‍රී ලංකා misspelled as ශ්රී ලංකා on the Web. This is
  sometimes due to ZWJ having been removed too eagerly, which turns the
  rakaransaya in ශ්‍රී (one visual unit) into ශ්රී (two visual units). Here we
  heuristically insert ZWJ in this context to restore the rakaransaya.

  Args:
    text: The text to which the heuristics will be applied.

  Returns:
    The modified text.
  """
  return LIKELY_RAKARANSAYA.sub('\u200d', text)


class TestSinhalaZeroWidth(unittest.TestCase):

  def test_RemoveOptionalZW(self):
    sri_lanka = '\u0dc1\u0dca\u200d\u0dbb\u0dd3 \u0dbd\u0d82\u0d9a\u0dcf'
    self.assertEqual(sri_lanka, 'ශ්‍රී ලංකා')
    self.assertEqual(RemoveOptionalZW(sri_lanka), sri_lanka)
    for i in range(len(sri_lanka) + 1):
      for zw in ('\u200c', '\u200d', '\u200c\u200d', '\u200d\u200c'):
        text = sri_lanka[:i] + zw + sri_lanka[i:]
        self.assertEqual(RemoveOptionalZW(text), sri_lanka)

    kaaryaala = ('\u0d9a\u0dcf\u0dbb\u0dca\u200d'
                 '\u0dba\u0dca\u200d\u0dba\u0dcf\u0dbd')
    self.assertEqual(kaaryaala, 'කාර්‍ය්‍යාල')
    self.assertEqual(
        RemoveOptionalZW(kaaryaala),
        '\u0d9a\u0dcf\u0dbb\u0dca\u200d\u0dba\u0dca\u200d\u0dba\u0dcf\u0dbd')
    self.assertEqual(RemoveOptionalZW('අකාර්යක්‍ෂමතාව'), 'අකාර්යක්ෂමතාව')
    self.assertEqual(RemoveOptionalZW('අත්තක්‌'), 'අත්තක්')
    self.assertEqual(RemoveOptionalZW('අනුයුක්‌තව'), 'අනුයුක්තව')

  def test_RemoveRepaya(self):
    self.assertEqual(RemoveRepaya('කාර්‍ය්‍යාල'), 'කාර්ය්‍යාල')

  def test_ForceRakaransaya(self):
    self.assertEqual(ForceRakaransaya('ශ්රී ලංකා'), 'ශ්‍රී ලංකා')
    self.assertEqual(ForceRakaransaya('බස් ඕල්ඩ්රින්'), 'බස් ඕල්ඩ්‍රින්')


if __name__ == '__main__':
  stdin = io.open(0, mode='rt', encoding='utf-8', closefd=False)
  stdout = io.open(1, mode='wt', encoding='utf-8', closefd=False)
  for line in stdin:
    stdout.write(RemoveOptionalZW(line))

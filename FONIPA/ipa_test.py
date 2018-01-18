# -*- coding: utf-8 -*-

# Copyright 2018 Google LLC. All Rights Reserved.
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

"""Tests for ipa."""

from __future__ import unicode_literals

import collections
import unicodedata
import unittest

import ipa

Equivalence = collections.namedtuple('Equivalence', ['ipa', 'nfc', 'other'])

IPA_EQUIVALENCES = [
    Equivalence(
        ipa='\u00E7',
        nfc='\u00E7',
        other=['c\u0327'],
    ),
    Equivalence(
        ipa='\u00E7\u0301',
        nfc='\u1E09',
        other=['c\u0301\u0327', 'c\u0327\u0301', '\u0107\u0327'],
    ),
    Equivalence(
        ipa='\u0259\u02DE',
        nfc='\u0259\u02DE',
        other=['\u025A'],
    ),
    Equivalence(
        ipa='\u0259\u0301\u02DE',
        nfc='\u0259\u0301\u02DE',
        other=['\u025A\u0301'],
    ),
    Equivalence(
        ipa='\u025C\u02DE',
        nfc='\u025C\u02DE',
        other=['\u025D'],
    ),
    Equivalence(
        ipa='\u025C\u0330\u0303\u02DE',
        nfc='\u025C\u0330\u0303\u02DE',
        other=['\u025D\u0330\u0303', '\u025D\u0303\u0330'],
    ),
    Equivalence(
        ipa='g',
        nfc='g',
        other=['\u0261'],
    ),
    Equivalence(
        ipa='g\u0306',
        nfc='\u011F',
        other=['\u0261\u0306'],
    ),
    Equivalence(
        ipa='o\u0303\u0301',
        nfc='\u1E4D',
        other=['\u00F5\u0301'],
    ),
    Equivalence(
        ipa='u\u0324\u0302',
        nfc='\u1E73\u0302',
        other=['u\u0302\u0324', '\u00FB\u0324'],
    )
]


def NFC(string):
  return unicodedata.normalize('NFC', string)


def NFD(string):
  return unicodedata.normalize('NFD', string)


class IpaTest(unittest.TestCase):

  def testIpaInventory(self):
    chars = set(ipa.BASE_LETTERS)
    chars.update(ipa.MODIFIER_LETTERS)
    chars.update(ipa.COMBINING_MARKS)
    for char in chars:
      if char != 'ç':
        self.assertEqual(NFD(char), char)
      if char != 'ɡ':
        self.assertEqual(ipa.Decompose(char), char)
    return

  def testDecompose(self):
    for equiv in IPA_EQUIVALENCES:
      self.assertEqual(NFC(equiv.ipa), equiv.nfc)
      self.assertEqual(NFC(equiv.nfc), equiv.nfc)
      for s in [equiv.ipa, equiv.nfc] + equiv.other:
        self.assertEqual(ipa.Decompose(s), equiv.ipa)
        self.assertEqual(ipa.Decompose(NFC(s)), equiv.ipa)
        self.assertEqual(ipa.Decompose(NFD(s)), equiv.ipa)
    return


if __name__ == '__main__':
  unittest.main()

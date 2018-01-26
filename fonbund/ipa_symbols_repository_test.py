# coding=utf-8

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

from __future__ import unicode_literals

import unittest

from fonbund import helpers
from fonbund import ipa_symbols_repository
from fonbund import segment_normalizer


class TestIpaSymbolsRepository(unittest.TestCase):

  def setUp(self):
    self._ipa_symbols_rep = ipa_symbols_repository.IpaSymbolsRepository()
    self.assertTrue(self._ipa_symbols_rep.Init())

  def test_can_initialize(self):
    ipa_symbols = self._ipa_symbols_rep.ipa_symbols
    self.assertTrue(ipa_symbols)
    # Reject empty symbol.
    self.assertFalse(self._ipa_symbols_rep.IsValidSymbol(""))
    # Accept voiced alveolar lateral fricative (single symbol).
    self.assertTrue(self._ipa_symbols_rep.IsValidSymbol("ɮ"))
    # Reject invalid IPA.
    self.assertFalse(self._ipa_symbols_rep.IsValidSymbol("Ⴟ"))
    # Reject a valid IPA string consisting of two symbols.
    self.assertFalse(self._ipa_symbols_rep.IsValidSymbol("a̤"))

    # Reject empty strings.
    self.assertFalse(self._ipa_symbols_rep.IsValidString(""))
    # Accept single IPA symbol as string.
    self.assertTrue(self._ipa_symbols_rep.IsValidString("ɮ"))
    # Accept a sequence of IPA symbols as string.
    self.assertTrue(self._ipa_symbols_rep.IsValidString("t̻x͡x↗"))
    self.assertTrue(self._ipa_symbols_rep.IsValidString("a̤"))
    # Reject a string that has invalid IPA in it.
    self.assertFalse(self._ipa_symbols_rep.IsValidString("ʧɳʌբեն"))
    self.assertFalse(self._ipa_symbols_rep.IsValidString("ميجابيت<U+200E>"))

    # Check description of an individual symbol.
    descriptions = self._ipa_symbols_rep.descriptions
    self.assertTrue(descriptions)
    description = []
    self.assertFalse(self._ipa_symbols_rep.GetDescription("+", description))
    self.assertTrue(self._ipa_symbols_rep.GetDescription("ʝ", description))
    self.assertEqual(1, len(description))
    self.assertEqual("voiced palatal fricative", description[0])
    # Check description of compound symbol.
    self.assertTrue(self._ipa_symbols_rep.GetDescription("æˤ", description))
    self.assertEqual(2, len(description))
    self.assertEqual("raised open front unrounded", description[0])
    self.assertEqual("pharyngealized", description[1])

  def test_check_all_phoible_segments(self):
    with helpers.GetResourceAsReader(
        "fonbund", "testdata/phoible_segments.txt") as reader:
      segments = reader.readlines()
    self.assertTrue(segments)
    invalid_segments = set()
    num_segments = 0
    for segment in segments:
      segment = segment.strip()
      if segment and segment[0] != "#":
        segment = segment_normalizer.SegmentNormalizer.NormalizeToNFD(segment)
        if not self._ipa_symbols_rep.IsValidString(segment):
          invalid_segments.add(segment)
        num_segments += 1
    self.assertTrue(num_segments)
    self.assertFalse(invalid_segments)


if __name__ == "__main__":
  unittest.main()

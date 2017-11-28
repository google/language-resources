#! /usr/bin/env python
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

"""Utilities for working with Bengali script.
"""

from __future__ import unicode_literals

import re

CODEPOINT_TO_SYMBOL = {
    # Punctuation and truncation symbols
    0x0027: 'apostrophe',
    0x002D: 'hyphen',
    0x02BC: 'urdha_comma',
    0x0964: 'dahri',
    0x0965: 'double_dahri',

    # Bengali (Bangla) Unicode block, per Unicode Standard version 8.0
    #
    # Various signs
    0x0980: 'anji',
    0x0981: 'candrabindu',
    0x0982: 'anusvara',
    0x0983: 'visarga',
    # Independent vowels
    0x0985: 'a',
    0x0986: 'aa',
    0x0987: 'i',
    0x0988: 'ii',
    0x0989: 'u',
    0x098A: 'uu',
    0x098B: 'ri',
    0x098C: 'li',
    0x098F: 'e',
    0x0990: 'ai',
    0x0993: 'o',
    0x0994: 'au',
    # Consonants
    0x0995: 'ka',
    0x0996: 'kha',
    0x0997: 'ga',
    0x0998: 'gha',
    0x0999: 'nga',
    0x099A: 'ca',
    0x099B: 'cha',
    0x099C: 'ja',
    0x099D: 'jha',
    0x099E: 'nya',
    0x099F: 'tta',
    0x09A0: 'ttha',
    0x09A1: 'dda',
    0x09A2: 'ddha',
    0x09A3: 'nna',
    0x09A4: 'ta',
    0x09A5: 'tha',
    0x09A6: 'da',
    0x09A7: 'dha',
    0x09A8: 'na',
    0x09AA: 'pa',
    0x09AB: 'pha',
    0x09AC: 'ba',
    0x09AD: 'bha',
    0x09AE: 'ma',
    0x09AF: 'ya',
    0x09B0: 'ra',
    0x09B2: 'la',
    0x09B6: 'sha',
    0x09B7: 'ssa',
    0x09B8: 'sa',
    0x09B9: 'ha',
    # Various signs
    0x09BC: 'nukta',
    0x09BD: 'avagraha',
    # Dependent vowel signs
    0x09BE: '-aa',
    0x09BF: '-i',
    0x09C0: '-ii',
    0x09C1: '-u',
    0x09C2: '-uu',
    0x09C3: '-ri',
    0x09C4: '-rri',
    0x09C7: '-e',
    0x09C8: '-ai',
    # Two-part dependent vowel signs
    0x09CB: '-o',
    0x09CC: '-au',
    # Virama
    0x09CD: 'hasant',
    # Additional consonant
    0x09CE: 't',
    # Sign
    0x09D7: '-au_length_mark',
    # Additional consonants
    0x09DC: 'rra',
    0x09DD: 'rha',
    0x09DF: 'yya',
    # Additional vowels for Sanskrit
    0x09E0: 'rri',
    0x09E1: 'lli',
    0x09E2: '-li',
    0x09E3: '-lli',
    # Digits
    0x09E6: '0',
    0x09E7: '1',
    0x09E8: '2',
    0x09E9: '3',
    0x09EA: '4',
    0x09EB: '5',
    0x09EC: '6',
    0x09ED: '7',
    0x09EE: '8',
    0x09EF: '9',
    # Additions for Assamese
    0x09F0: 'ra_with_middle_diagonal',
    0x09F1: 'ra_with_lower_diagonal',
    # Currency signs
    0x09F2: 'rupee_mark',
    0x09F3: 'rupee_sign',
    # Historic symbols for fractional values
    0x09F4: 'currency_numerator_one',
    0x09F5: 'currency_numerator_two',
    0x09F6: 'currency_numerator_three',
    0x09F7: 'currency_numerator_four',
    0x09F8: 'currency_numerator_one_less_than_the_denominator',
    0x09F9: 'currency_denominator_sixteen',
    # Sign
    0x09FA: 'isshar',
    # Historic currency sign
    0x09FB: 'ganda',

    # Zero-width non-joiner and zero-width joiner
    0x200C: 'ZWNJ',
    0x200D: 'ZWJ',

    # Private-use characters
    0xE000: '-a',         # inherent vowel
    0xE001: 'ya_phalaa',  # jo-fola
}


TOKEN_RE = re.compile(r'%s(?:[-\u0027\u02BC\u200C\u200D]*%s)*' %
                      ((r'[\u0980-\u09FF]',) * 2))

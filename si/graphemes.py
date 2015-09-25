#! /usr/bin/env python2
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

"""Utilities for working with Sinhala script.
"""

from __future__ import unicode_literals

import re

CODEPOINT_TO_SYMBOL = {
    # Latin alphabet
    0x0046: 'latin_F',
    0x005A: 'latin_Z',
    0x0066: 'latin_f',
    0x007A: 'latin_z',

    # Punctuation symbols
    0x0964: 'danda',
    0x0965: 'double_danda',

    # Sinhala Unicode block, per Unicode Standard version 8.0
    #
    # Various signs
    0x0D82: 'anusvara',
    0x0D83: 'visarga',
    # Independent vowels
    0x0D85: 'a',
    0x0D86: 'aa',
    0x0D87: 'ae',
    0x0D88: 'aae',
    0x0D89: 'i',
    0x0D8A: 'ii',
    0x0D8B: 'u',
    0x0D8C: 'uu',
    0x0D8D: 'ru',
    0x0D8E: 'rru',
    0x0D8F: 'lu',
    0x0D90: 'llu',
    0x0D91: 'e',
    0x0D92: 'ee',
    0x0D93: 'ai',
    0x0D94: 'o',
    0x0D95: 'oo',
    0x0D96: 'au',
    # Consonants
    0x0D9A: 'ka',
    0x0D9B: 'kha',
    0x0D9C: 'ga',
    0x0D9D: 'gha',
    0x0D9E: 'nga',
    0x0D9F: 'nnga',
    0x0DA0: 'ca',
    0x0DA1: 'cha',
    0x0DA2: 'ja',
    0x0DA3: 'jha',
    0x0DA4: 'nya',
    0x0DA5: 'jnya',
    0x0DA6: 'nyja',
    0x0DA7: 'tta',
    0x0DA8: 'ttha',
    0x0DA9: 'dda',
    0x0DAA: 'ddha',
    0x0DAB: 'nna',
    0x0DAC: 'nndda',
    0x0DAD: 'ta',
    0x0DAE: 'tha',
    0x0DAF: 'da',
    0x0DB0: 'dha',
    0x0DB1: 'na',
    # 0x0DB2 <reserved>
    0x0DB3: 'nda',
    0x0DB4: 'pa',
    0x0DB5: 'pha',
    0x0DB6: 'ba',
    0x0DB7: 'bha',
    0x0DB8: 'ma',
    0x0DB9: 'mba',
    0x0DBA: 'ya',
    0x0DBB: 'ra',
    # 0x0DBC <reserved>
    0x0DBD: 'la',
    # 0x0DBE <reserved>
    # 0x0DBF <reserved>
    0x0DC0: 'va',
    0x0DC1: 'sha',
    0x0DC2: 'ssa',
    0x0DC3: 'sa',
    0x0DC4: 'ha',
    0x0DC5: 'lla',
    0x0DC6: 'fa',
    # Sign
    0x0DCA: 'virama',
    # Dependent vowel signs
    0x0DCF: '-aa',
    0x0DD0: '-ae',
    0x0DD1: '-aae',
    0x0DD2: '-i',
    0x0DD3: '-ii',
    0x0DD4: '-u',
    # 0x0DD5 <reserved>
    0x0DD6: '-uu',
    # 0x0DD7 <reserved>
    0x0DD8: '-ru',
    0x0DD9: '-e',
    0x0DDA: '-ee',
    0x0DDB: '-ai',
    # Two-part dependent vowel signs
    0x0DDC: '-o',
    0x0DDD: '-oo',
    0x0DDE: '-au',
    # Dependent vowel sign
    0x0DDF: '-lu',
    # Astrological digits
    0x0DE6: 'lith_0',
    0x0DE7: 'lith_1',
    0x0DE8: 'lith_2',
    0x0DE9: 'lith_3',
    0x0DEA: 'lith_4',
    0x0DEB: 'lith_5',
    0x0DEC: 'lith_6',
    0x0DED: 'lith_7',
    0x0DEE: 'lith_8',
    0x0DEF: 'lith_9',
    # Additional dependent vowel signs
    0x0DF2: '-rru',
    0x0DF3: '-llu',
    # Punctuation
    0x0DF4: 'kunddaliya',

    # Zero-width non-joiner and zero-width joiner
    0x200C: 'ZWNJ',
    0x200D: 'ZWJ',

    # Sinhala Archaic Numbers Unicode block, per Unicode Standard version 8.0
    #
    # Historical digits
    0x111E1: 'archaic_1',
    0x111E2: 'archaic_2',
    0x111E3: 'archaic_3',
    0x111E4: 'archaic_4',
    0x111E5: 'archaic_5',
    0x111E6: 'archaic_6',
    0x111E7: 'archaic_7',
    0x111E8: 'archaic_8',
    0x111E9: 'archaic_9',
    # Historical numbers
    0x111EA: 'archaic_10',
    0x111EB: 'archaic_20',
    0x111EC: 'archaic_30',
    0x111ED: 'archaic_40',
    0x111EE: 'archaic_50',
    0x111EF: 'archaic_60',
    0x111F0: 'archaic_70',
    0x111F1: 'archaic_80',
    0x111F2: 'archaic_90',
    0x111F3: 'archaic_100',
    0x111F4: 'archaic_1000',
}


TOKEN_RE = re.compile(r'%s(?:[\u200C\u200D]*%s)*' %
                      ((r'[fFzZ\u0D80-\u0DFF]',) * 2))

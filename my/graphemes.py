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

"""Utilities for working with Burmese/Myanmar script.
"""

from __future__ import unicode_literals

import re

CODEPOINT_TO_SYMBOL = {
    # Myanmar Unicode block, per Unicode Standard version 8.0
    #
    0x1000: 'ka',
    0x1001: 'kha',
    0x1002: 'ga',
    0x1003: 'gha',
    0x1004: 'nga',
    0x1005: 'ca',
    0x1006: 'cha',
    0x1007: 'ja',
    0x1008: 'jha',
    0x1009: 'nya',
    0x100A: 'nnya',
    0x100B: 'tta',
    0x100C: 'ttha',
    0x100D: 'dda',
    0x100E: 'ddha',
    0x100F: 'nna',
    0x1010: 'ta',
    0x1011: 'tha',
    0x1012: 'da',
    0x1013: 'dha',
    0x1014: 'na',
    0x1015: 'pa',
    0x1016: 'pha',
    0x1017: 'ba',
    0x1018: 'bha',
    0x1019: 'ma',
    0x101A: 'ya',
    0x101B: 'ra',
    0x101C: 'la',
    0x101D: 'wa',
    0x101E: 'sa',
    0x101F: 'ha',
    0x1020: 'lla',
    0x1021: 'a',
    0x1022: 'shan_a',
    0x1023: 'i',
    0x1024: 'ii',
    0x1025: 'u',
    0x1026: 'uu',
    0x1027: 'e',
    0x1028: 'mon_e',
    0x1029: 'o',
    0x102A: 'au',
    0x102B: '-AA',
    0x102C: '-aa',
    0x102D: '-i',
    0x102E: '-ii',
    0x102F: '-u',
    0x1030: '-uu',
    0x1031: '-e',
    0x1032: '-ai',
    0x1033: '-mon_ii',
    0x1034: '-mon_o',
    0x1035: '-e_above',
    0x1036: 'anusvara',
    0x1037: 'dot',
    0x1038: 'visarga',
    0x1039: 'virama',
    0x103A: 'asat',
    0x103B: '-y-',
    0x103C: '-r-',
    0x103D: '-w-',
    0x103E: '-h-',
    0x103F: 'great_sa',
    0x1040: '0',
    0x1041: '1',
    0x1042: '2',
    0x1043: '3',
    0x1044: '4',
    0x1045: '5',
    0x1046: '6',
    0x1047: '7',
    0x1048: '8',
    0x1049: '9',
    0x104A: 'little_section',
    0x104B: 'section',
    0x104C: 'locative',
    0x104D: 'completed',
    0x104E: 'aforementioned',
    0x104F: 'genitive',
    # Pali and Sanskrit extensions
    0x1050: 'sha',
    0x1051: 'ssa',
    0x1052: 'ri',
    0x1053: 'rri',
    0x1054: 'li',
    0x1055: 'lli',
    0x1056: '-ri',
    0x1057: '-rri',
    0x1058: '-li',
    0x1059: '-lli',
}


TOKEN_RE = re.compile(r'[\u1000-\u1059]+')

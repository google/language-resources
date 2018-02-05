# -*- coding: utf-8 -*-

# Copyright 2016, 2017, 2018 Google Inc. All Rights Reserved.
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

"""Symbolize Hindustani in Perso-Arabic script (ISO 15924 "Arab").
"""

from __future__ import unicode_literals

import re
import sys
import unicodedata

from utils import utf8

ARAB_CODEPOINT_TO_SYMBOL = {
    # Letters used in Indo-European vocabulary have short symbol names:
    0x0627: "|",   # alif
    0x0622: "AA",  # alif madd
    0x0628: "b",   # be
    0x067E: "p",   # pe
    0x062A: "t",   # te
    0x0679: "tt",  # ṭe
    0x062C: "j",   # jīm
    0x0686: "c",   # ce
    0x062E: "Kh",  # khe
    0x062F: "d",   # dāl
    0x0688: "dd",  # ḍāl
    0x0631: "r",   # re
    0x0691: "rr",  # ṛe
    0x0632: "z",   # ze
    0x0698: "zh",  # zhe
    0x0633: "s",   # sīn
    0x0634: "sh",  # shīn
    0x063A: "Gh",  # ghain
    0x0641: "f",   # fe
    0x0642: "q",   # qāf
    0x06A9: "k",   # kāf (ARABIC LETTER KEHEH)
    0x06AF: "g",   # gāf
    0x0644: "l",   # lām
    0x0645: "m",   # mīm
    0x0646: "n",   # nūn
    0x06BA: "N",   # nūn ghunnah Letter (ARABIC LETTER NOON GHUNNA)
    0x0658: "-N",  # nūn ghunnah Modifier (ARABIC MARK NOON GHUNNA)
    0x0648: "w",   # vā'o
    0x0624: "'w",  # vā'o with hamzah
    0x06C1: "h",   # choṭī/gol he             (HEH GOAL)
    0x06C2: "'h",  # choṭī/gol he with hamzah (HEH GOAL WITH HAMZA ABOVE)
    0x06BE: "+h",  # do cashmī he             (HEH DOACHASHMEE)
    0x0621: "'",   # hamzah
    0x06CC: "y",   # choṭī ye
    0x0626: "'y",  # choṭī ye with hamzah
    0x06D2: "Y",   # baṛī ye             (YEH BARREE)
    0x06D3: "'Y",  # baṛī ye with hamzah (YEH BARREE WITH HAMZA ABOVE)

    # Modifier diacritics have short symbol names:
    0x064E: "a",   # zabar (FATHA)
    0x064F: "u",   # pesh (DAMMA)
    0x0650: "i",   # zer (KASRA)
    0x0651: ":",   # tashdīd (SHADDA)
    0x0652: "0",   # jazm (SUKUN)
    0x0654: "-'",  # HAMZA ABOVE Modifier
    0x0670: "-|",  # SUPERSCRIPT ALEF Modifier

    # Characters used in loanwords from Arabic etc. have longer symbol names:
    0x064B: "tanwin",   # tanwīn (ARABIC FATHATAN)
    0x062B: "se",       # s̱e (ARABIC LETTER THEH)
    0x062D: "H",        # baṛī ḥe (ARABIC LETTER HAH)
    0x0630: "zal",      # ẕāl (ARABIC LETTER THAL)
    0x0635: "swad",     # ṣwād (ARABIC LETTER SAD)
    0x0636: "zwad",     # ẓwād (ARABIC LETTER DAD)
    0x0637: "toe",      # t̤o'e (ARABIC LETTER TAH)
    0x0638: "zoe",      # z̤o'e (ARABIC LETTER ZAH)
    0x0639: "ain",      # ʿain (ARABIC LETTER AIN)

    # The following characters are not used at all in Urdu:
    # 0623: "'|",             ARABIC LETTER ALEF WITH HAMZA ABOVE
    # 0629: "te_marbuta",     ARABIC LETTER TEH MARBUTA
    # 0643: "arabic_kaf",     ARABIC LETTER KAF              use 06A9 instead
    # 0647: "arabic_he",      ARABIC LETTER HEH              use 06C1 instead
    # 0649: "alif_maksura",   ARABIC LETTER ALEF MAKSURA     use 06CC instead
    # 064A: "arabic_ye",      ARABIC LETTER YEH              use 06CC instead
    # 06C3: "gol_te_marbuta", ARABIC LETTER TEH MARBUTA GOAL
}

ZERO_WIDTH = re.compile(r"[\u200B-\u200F]+")
REORDER_SHADDA = re.compile(r"([\u064E-\u0650]+)\u0651")


def Symbolize(string, cp2sym=ARAB_CODEPOINT_TO_SYMBOL):
  string = ZERO_WIDTH.sub("", string)
  string = REORDER_SHADDA.sub("\u0651\\1", string)
  return " ".join(cp2sym[ord(c)] for c in string)


def PrintSymbols(symtab, prefix="in"):
  for cp in sorted(symtab):
    utf8.Print('%sput_symbol { value: 0x%04X key: "%s" }'
               % (prefix, cp, symtab[cp]))
  return


def main(unused_argv):
  for line in utf8.stdin:
    line = line.rstrip("\n")
    if not line or line.startswith("#"):
      continue
    line = unicodedata.normalize("NFC", line)
    fields = line.split("\t")
    assert fields
    arab = fields[0]
    arab = arab.replace(" ", "")
    arab_sym = Symbolize(arab)
    utf8.Print("\t".join([arab, arab_sym] + fields[1:]))
  return


if __name__ == "__main__":
  main(sys.argv)

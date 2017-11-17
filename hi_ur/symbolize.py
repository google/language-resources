# -*- coding: utf-8 -*-
#
# Copyright 2016, 2017 Google Inc. All Rights Reserved.
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

"""Symbolize Hindustani parallel script data (Devanagari/Arabic).
"""

from __future__ import unicode_literals

import re
import sys
import unicodedata

from utils import utf8


DEVA_CODEPOINT_TO_SYMBOL = {
    # Various signs
    0x0901: "candrabindu",
    0x0902: "anusvara",
    0x0903: "visarga",
    # Independent vowels
    0x0905: "a",
    0x0906: "aa",
    0x0907: "i",
    0x0908: "ii",
    0x0909: "u",
    0x090A: "uu",
    0x090B: "ri",
    0x090D: "candra_e",
    0x090F: "ee",
    0x0910: "ai",
    0x0911: "candra_o",
    0x0913: "oo",
    0x0914: "au",
    # Consonants
    0x0915: "ka",
    0x0916: "kha",
    0x0917: "ga",
    0x0918: "gha",
    0x0919: "nga",
    0x091A: "ca",
    0x091B: "cha",
    0x091C: "ja",
    0x091D: "jha",
    0x091E: "nya",
    0x091F: "tta",
    0x0920: "ttha",
    0x0921: "dda",
    0x0922: "ddha",
    0x0923: "nna",
    0x0924: "ta",
    0x0925: "tha",
    0x0926: "da",
    0x0927: "dha",
    0x0928: "na",
    0x092A: "pa",
    0x092B: "pha",
    0x092C: "ba",
    0x092D: "bha",
    0x092E: "ma",
    0x092F: "ya",
    0x0930: "ra",
    0x0932: "la",
    0x0933: "lla",
    0x0935: "va",
    0x0936: "sha",
    0x0937: "ssa",
    0x0938: "sa",
    0x0939: "ha",
    # Various signs
    0x093C: "nukta",
    # Dependent vowel signs
    0x093E: "-aa",
    0x093F: "-i",
    0x0940: "-ii",
    0x0941: "-u",
    0x0942: "-uu",
    0x0943: "-ri",
    0x0945: "-candra_e",
    0x0947: "-ee",
    0x0948: "-ai",
    0x0949: "-candra_o",
    0x094B: "-oo",
    0x094C: "-au",
    # Virama
    0x094D: "virama",
}

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
    0X064F: "u",   # pesh (DAMMA)
    0x0650: "i",   # zer (KASRA)
    0x0651: ":",   # tashdīd (SHADDA)
    0x0652: "0",   # jazm (SUKUN)
    0x0654: "-'",  # HAMZA ABOVE Modifier
    0x0670: "-|",  # SUPERSCRIPT ALEF Modifier

    # Characters used in loanwords from Arabic etc. have longer symbol names:
    0x064B: "tanwin",   # tanwīn (ARABIC FATHATAN)
    0x062B: "se",       # s̱e (ARABIC LETTER THEH)
    0x062D: "bari_he",  # baṛī ḥe (ARABIC LETTER HAH)
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


def Symbolize(string, cp2sym):
  string = ZERO_WIDTH.sub("", string)
  string = REORDER_SHADDA.sub("\u0651\\1", string)
  return " ".join(cp2sym[ord(c)] for c in string)


def PrintSymbols(symtab, prefix="in"):
  for cp in sorted(symtab):
    utf8.Print('%sput_symbol { value: 0x%04X key: "%s" }'
               % (prefix, cp, symtab[cp]))
  return


def main(argv):
  try:
    deva_index = int(argv[1])
  except:  # pylint: disable=bare-except
    deva_index = 0

  try:
    arab_index = int(argv[2])
  except:  # pylint: disable=bare-except
    arab_index = 1

  success = True
  for line in utf8.stdin:
    line = line.rstrip("\n")
    if not line or line.startswith("#"):
      continue
    line = unicodedata.normalize("NFC", line)
    fields = line.split("\t")
    assert len(fields) > max(deva_index, arab_index)
    deva = fields[deva_index]
    arab = fields[arab_index].replace(" ", "")
    try:
      deva_sym = Symbolize(deva, DEVA_CODEPOINT_TO_SYMBOL)
      arab_sym = Symbolize(arab, ARAB_CODEPOINT_TO_SYMBOL)
      utf8.Print("\t".join([deva, deva_sym, arab_sym, arab]))
    except Exception as e:  # pylint: disable=broad-except
      utf8.Print("Error symbolizing line %s: %s" % (line, e),
                 file=utf8.stderr)
      success = False
  sys.exit(0 if success else 1)
  return


if __name__ == "__main__":
  main(sys.argv)

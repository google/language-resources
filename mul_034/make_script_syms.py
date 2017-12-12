#! /usr/bin/env python

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
#
# Copyright 2017 Google LLC. All Rights Reserved.

"""Writes OpenFst SymbolTables for various South (East) Asian scripts.
"""

from __future__ import unicode_literals

__author__ = 'mjansche@google.com (Martin Jansche)'

import io
import sys

import icu  # pip install PyICU

STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)
STDERR = io.open(2, mode='wt', encoding='utf-8', closefd=False)

STOPWORDS = frozenset([
    'ACCENT',
    'CHARACTER',
    'COMBINING',
    'CONSONANT',
    'CURRENCY',
    'DIGIT',
    'FRACTION',
    'INDEPENDENT',
    'INHERENT',
    'LETTER',
    'LOGOGRAM',
    'LOGOTYPE',
    'MARK',
    'MEDIAL',
    'MODIFIER',
    'NUMBER',
    'PUNCTUATION',
    'SARA',
    'SEMIVOWEL',
    'SIGN',
    'STRESS',
    'SUBJOINED',
    'SYLLABLE',
    'SYMBOL',
    'TONE',
    'VOWEL',
])

DIGITS = {
    'DIGIT ZERO': '0',
    'DIGIT ONE': '1',
    'DIGIT TWO': '2',
    'DIGIT THREE': '3',
    'DIGIT FOUR': '4',
    'DIGIT FIVE': '5',
    'DIGIT SIX': '6',
    'DIGIT SEVEN': '7',
    'DIGIT EIGHT': '8',
    'DIGIT NINE': '9',
}

EXCEPTIONS = {
    0x09F2: 'rupee_mark',
    0x09F3: 'rupee_sign',
    0x0BFA: 'number',
    0x109E: 'symbol_shan_one',
    0xA8EB: '-letter_u',
    0x110BD: 'number',
    0x11131: '-o_mark',
    0x11132: '-au_mark',
    0x111CB: 'vowel_modifier',
}

SINHALA_CHAR_NAMES = {
    0x0D82: 'sinhala sign anusvara',
    0x0D83: 'sinhala sign visarga',
    0x0D85: 'sinhala letter a',
    0x0D86: 'sinhala letter aa',
    0x0D87: 'sinhala letter ae',
    0x0D88: 'sinhala letter aae',
    0x0D89: 'sinhala letter i',
    0x0D8a: 'sinhala letter ii',
    0x0D8b: 'sinhala letter u',
    0x0D8c: 'sinhala letter uu',
    0x0D8d: 'sinhala letter vocalic r',
    0x0D8e: 'sinhala letter vocalic rr',
    0x0D8f: 'sinhala letter vocalic l',
    0x0D90: 'sinhala letter vocalic ll',
    0x0D91: 'sinhala letter e',
    0x0D92: 'sinhala letter ee',
    0x0D93: 'sinhala letter ai',
    0x0D94: 'sinhala letter o',
    0x0D95: 'sinhala letter oo',
    0x0D96: 'sinhala letter au',
    0x0D9a: 'sinhala letter ka',
    0x0D9b: 'sinhala letter kha',
    0x0D9c: 'sinhala letter ga',
    0x0D9d: 'sinhala letter gha',
    0x0D9e: 'sinhala letter nga',
    0x0D9F: 'sinhala letter nnga',
    0x0DA0: 'sinhala letter ca',
    0x0DA1: 'sinhala letter cha',
    0x0DA2: 'sinhala letter ja',
    0x0DA3: 'sinhala letter jha',
    0x0DA4: 'sinhala letter nya',
    0x0DA5: 'sinhala letter jnya',
    0x0DA6: 'sinhala letter nyja',
    0x0DA7: 'sinhala letter tta',
    0x0DA8: 'sinhala letter ttha',
    0x0DA9: 'sinhala letter dda',
    0x0DAA: 'sinhala letter ddha',
    0x0DAB: 'sinhala letter nna',
    0x0DAC: 'sinhala letter nndda',
    0x0DAD: 'sinhala letter ta',
    0x0DAE: 'sinhala letter tha',
    0x0DAF: 'sinhala letter da',
    0x0DB0: 'sinhala letter dha',
    0x0DB1: 'sinhala letter na',
    0x0DB3: 'sinhala letter nda',
    0x0DB4: 'sinhala letter pa',
    0x0DB5: 'sinhala letter pha',
    0x0DB6: 'sinhala letter ba',
    0x0DB7: 'sinhala letter bha',
    0x0DB8: 'sinhala letter ma',
    0x0DB9: 'sinhala letter mba',
    0x0DBA: 'sinhala letter ya',
    0x0DBB: 'sinhala letter ra',
    0x0DBD: 'sinhala letter la',
    0x0DC0: 'sinhala letter va',
    0x0DC1: 'sinhala letter sha',
    0x0DC2: 'sinhala letter ssa',
    0x0DC3: 'sinhala letter sa',
    0x0DC4: 'sinhala letter ha',
    0x0DC5: 'sinhala letter lla',
    0x0DC6: 'sinhala letter fa',
    0x0DCA: 'sinhala sign virama',
    0x0DCF: 'sinhala vowel sign aa',
    0x0DD0: 'sinhala vowel sign ae',
    0x0DD1: 'sinhala vowel sign aae',
    0x0DD2: 'sinhala vowel sign i',
    0x0DD3: 'sinhala vowel sign ii',
    0x0DD4: 'sinhala vowel sign u',
    0x0DD6: 'sinhala vowel sign uu',
    0x0DD8: 'sinhala vowel sign vocalic r',
    0x0DD9: 'sinhala vowel sign e',
    0x0DDA: 'sinhala vowel sign ee',
    0x0DDB: 'sinhala vowel sign ai',
    0x0DDC: 'sinhala vowel sign o',
    0x0DDD: 'sinhala vowel sign oo',
    0x0DDE: 'sinhala vowel sign au',
    0x0DDF: 'sinhala vowel sign vocalic l',
    0x0DF2: 'sinhala vowel sign vocalic rr',
    0x0DF3: 'sinhala vowel sign vocalic ll',
}

BRAHMIC = [
    'Deva',
    'Beng',
    'Guru',
    'Gujr',
    'Orya',
    'Taml',
    'Telu',
    'Knda',
    'Mlym',
    'Sinh',
    'Thai',
    'Laoo',
    'Tibt',
    'Mymr',
    'Tglg',
    'Hano',
    'Buhd',
    'Tagb',
    'Khmr',
    'Limb',
    'Tale',
    'Talu',
    'Bugi',
    'Lana',
    'Bali',
    'Sund',
    'Batk',
    'Lepc',
    'Sylo',
    'Phag',
    'Saur',
    'Rjng',
    'Java',
    'Cham',
    'Tavt',
    'Mtei',
    'Kthi',
    'Cakm',
    'Mahj',
    'Shrd',
    'Khoj',
    'Mult',
    'Sind',
    'Gran',
    'Newa',
    'Tirh',
    'Sidd',
    'Modi',
    'Takr',
    'Ahom',
    'Bhks',
    'Marc',
]


def CharName(c):
  cp = ord(c)
  if cp in SINHALA_CHAR_NAMES:
    return SINHALA_CHAR_NAMES[cp].upper()
  return icu.Char.charName(c)


def SymbolNames(script, include_script_code=False):
  """Yields short symbolic names for all characters in the given script."""
  script_name = script.getName()
  script_chars = icu.UnicodeSet(r'\p{%s}' % script_name)
  for c in script_chars:
    if ord(c) in EXCEPTIONS:
      short_name = EXCEPTIONS[ord(c)]
    else:
      name = CharName(c)
      if not name:
        continue
      prefix = script_name.upper().replace('_', ' ')
      if name.startswith(prefix):
        name = name[len(prefix):]
      assert name
      for old, new in DIGITS.items():
        name = name.replace(old, new)
      components = [t for t in name.split() if t not in STOPWORDS]
      short_name = '_'.join(components).lower()
      assert short_name, ('Empty symbolic name for %04X (%s)' %
                          (ord(c), CharName(c)))
      if short_name.startswith('-'):
        short_name = "'%s" % short_name[1:]
      if ('VOWEL SIGN' in name or 'CONSONANT SIGN' in name or 'MARK' in name or
          'MEDIAL' in name or 'COMBINING' in name or 'SUBJOINED' in name or
          'SARA' in name):
        short_name = '-%s' % short_name
    if include_script_code:
      short_name = '%s:%s' % (script.getShortName(), short_name)
    yield c, short_name
  yield '\u200C', 'ZWNJ'
  yield '\u200D', 'ZWJ'
  return


def GetScript(raw_script_name):
  """Returns the canonical long script name for a given raw name."""
  script_codes = icu.Script.getCode(raw_script_name)
  if len(script_codes) != 1:
    STDERR.write('Unknown or ambiguous script name: %s\n' % raw_script_name)
    return None
  return icu.Script(script_codes[0])


def IsBijectiveMapping(syms):
  """Checks if the given character-to-symbol mapping is bijective."""
  ok = True
  sym2char = {}
  for c, sym in syms:
    if sym in sym2char:
      b = sym2char[sym]
      STDERR.write('%04X (%s) and %04X (%s) both map to "%s"\n' %
                   (ord(b), CharName(b),
                    ord(c), CharName(c), sym))
      ok = False
    else:
      sym2char[sym] = c
  return ok


def WriteOpenFstSymbolTable(writer, syms):
  """Writes an OpenFst SymbolTable in text format to the given writer."""
  writer.write('<epsilon>\t0\n')
  for c, sym in syms:
    writer.write('%s\t%d\n' % (sym, ord(c)))
  writer.flush()
  return


def main(argv):
  if len(argv) < 2:
    STDOUT.write('Usage: %s [Script...]\n' % argv[0])
    sys.exit(2)

  if argv[1] == '--brahmic':
    scripts = BRAHMIC + argv[2:]
  elif argv[1] == '--india':
    scripts = BRAHMIC[:9] + argv[2:]
  else:
    scripts = argv[1:]
  success = True
  for s in scripts:
    script = GetScript(s)
    if not script:
      success = False
      continue
    syms = list(SymbolNames(script))
    if not IsBijectiveMapping(syms):
      success = False
      continue
    filename = '%s.syms' % script.getName()
    with io.open(filename, mode='wt', encoding='utf-8') as writer:
      WriteOpenFstSymbolTable(writer, syms)
    STDERR.write('Wrote %s\n' % filename)
  if success:
    STDERR.write('PASS\n')
    sys.exit(0)
  else:
    STDERR.write('FAIL\n')
    sys.exit(1)
  return


if __name__ == '__main__':
  main(sys.argv)

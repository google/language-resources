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
    'VOCALIC',
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


def SymbolNames(script, include_script_code=False):
  """Yields short symbolic names for all characters in the given script."""
  script_name = script.getName()
  script_chars = icu.UnicodeSet(r'\p{%s}' % script_name)
  for c in script_chars:
    if ord(c) in EXCEPTIONS:
      yield c, EXCEPTIONS[ord(c)]
      continue
    name = icu.Char.charName(c)
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
                        (ord(c), icu.Char.charName(c)))
    if short_name.startswith('-'):
      short_name = "'%s" % short_name[1:]
    if 'VOCALIC' in name:
      short_name += 'i'
    if ('VOWEL SIGN' in name or 'CONSONANT SIGN' in name or 'MARK' in name or
        'MEDIAL' in name or 'COMBINING' in name or 'SUBJOINED' in name or
        'SARA' in name):
      short_name = '-%s' % short_name
    if include_script_code:
      short_name = '%s_%s' % (short_name, script.getShortName())
    yield c, short_name
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
                   (ord(b), icu.Char.charName(b),
                    ord(c), icu.Char.charName(c), sym))
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

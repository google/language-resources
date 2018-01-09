#! /usr/bin/env python
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

"""Utilities for working with the International Phonetic Alphabet."""

from __future__ import unicode_literals

import collections
import io

# IPA Handbook, Table 3
IPA_LETTERS = {
    'a': 'open front unrounded vowel',
    'ɐ': 'near-open central vowel',
    'ɑ': 'open back unrounded vowel',
    'ɒ': 'open back rounded vowel',
    'æ': 'near-open front unrounded vowel',
    'b': 'voiced bilabial plosive',
    'ɓ': 'voiced bilabial implosive',
    'ʙ': 'voiced bilabial trill',
    'β': 'voiced bilabial fricative',
    'c': 'voiceless palatal plosive',
    'ç': 'voiceless palatal fricative',
    'ɕ': 'voiceless alveolo-palatal fricative',
    'd': 'voiced dental|alveolar plosive',
    'ɗ': 'voiced dental|alveolar implosive',
    'ɖ': 'voiced retroflex plosive',
    'ᶑ': 'voiced retroflex implosive',  # nonstandard; [ɖ] suffices
    'ð': 'voiced dental fricative',
    'e': 'close-mid front unrounded vowel',
    'ə': 'mid central vowel',
    # 'ᵊ' see below
    'ɘ': 'close-mid central unrounded vowel',
    'ɛ': 'open-mid front unrounded vowel',
    'ɜ': 'open-mid central unrounded vowel',
    'ɞ': 'open-mid central rounded vowel',
    'f': 'voiceless labiodental fricative',
    'g': 'voiced velar plosive',
    'ɠ': 'voiced velar implosive',
    'ɢ': 'voiced uvular plosive',
    'ʛ': 'voiced uvular implosive',
    'ɣ': 'voiced velar fricative',
    # 'ˠ' see below
    'ɤ': 'close-mid back unrounded vowel',
    'h': 'voiceless glottal fricative',
    # 'ʰ' see below
    'ħ': 'voiceless pharyngeal fricative',
    'ɦ': 'voiced glottal fricative',
    'ɧ': 'voiceless postalveolar velar fricative',
    'ɥ': 'voiced bilabial palatal approximant',  # was: labial
    'ʜ': 'voiceless epiglottal fricative',
    'i': 'close front unrounded vowel',
    'ɨ': 'close central unrounded vowel',
    'ɪ': 'near-close near-front unrounded vowel',
    'j': 'voiced palatal approximant',
    # 'ʲ' see below
    'ʝ': 'voiced palatal fricative',
    'ɟ': 'voiced palatal plosive',
    'ʄ': 'voiced palatal implosive',
    'k': 'voiceless velar plosive',
    'l': 'voiced dental|alveolar lateral approximant',
    # 'ˡ' see below
    'ɫ': 'velar voiced dental|alveolar lateral approximant',  # was: velarized
    'ɬ': 'voiceless dental|alveolar lateral fricative',
    'ɭ': 'voiced retroflex lateral approximant',
    'ʟ': 'voiced velar lateral approximant',
    'ɮ': 'voiced dental|alveolar lateral fricative',
    'm': 'voiced bilabial nasal',
    'ɱ': 'voiced labiodental nasal',
    'ɯ': 'close back unrounded vowel',
    'ɰ': 'voiced velar approximant',
    'n': 'voiced dental|alveolar nasal',
    # 'ⁿ' see below
    'ɲ': 'voiced palatal nasal',
    'ŋ': 'voiced velar nasal',
    'ɳ': 'voiced retroflex nasal',
    'ɴ': 'voiced uvular nasal',
    'o': 'close-mid back rounded vowel',
    'ʘ': 'bilabial click',
    'ɵ': 'close-mid central rounded vowel',
    'ø': 'close-mid front rounded vowel',
    'œ': 'open-mid front rounded vowel',
    'ɶ': 'open front rounded vowel',
    'ɔ': 'open-mid back rounded vowel',
    'p': 'voiceless bilabial plosive',
    'ɸ': 'voiceless bilabial fricative',
    'q': 'voiceless uvular plosive',
    'r': 'voiced dental|alveolar trill',
    'ɾ': 'voiced dental|alveolar flap',  # was: tap
    'ɺ': 'voiced alveolar lateral flap',
    'ɽ': 'voiced retroflex flap',
    'ɹ': 'voiced dental|alveolar approximant',
    'ɻ': 'voiced retroflex approximant',
    'ʀ': 'voiced uvular trill',
    'ʁ': 'voiced uvular fricative',
    's': 'voiceless alveolar fricative',
    'ʂ': 'voiceless retroflex fricative',
    'ʃ': 'voiceless postalveolar fricative',
    't': 'voiceless dental|alveolar plosive',
    'ʈ': 'voiceless retroflex plosive',
    'θ': 'voiceless dental fricative',
    # 'ᶿ' see below
    'u': 'close back rounded vowel',
    'ʉ': 'close central rounded vowel',
    'ʊ': 'near-close near-back rounded vowel',
    'v': 'voiced labiodental fricative',
    'ʋ': 'voiced labiodental approximant',
    'ʌ': 'open-mid back unrounded vowel',
    'ⱱ': 'labiodental flap',
    'w': 'voiced bilabial velar approximant',  # was: labial
    # 'ʷ' see below
    'ʍ': 'voiceless bilabial velar fricative',  # was: labial
    'x': 'voiceless velar fricative',
    # 'ˣ' see below
    'χ': 'voiceless uvular fricative',
    'y': 'close front rounded vowel',
    'ʎ': 'voiced palatal lateral approximant',
    'ʏ': 'near-close near-front rounded vowel',
    'z': 'voiced alveolar fricative',
    'ʑ': 'voiced alveolo-palatal fricative',
    'ʐ': 'voiced retroflex fricative',
    'ʒ': 'voiced postalveolar fricative',
    'ʔ': 'glottal plosive',
    'ʡ': 'epiglottal plosive',
    'ʕ': 'voiced pharyngeal fricative',  # was: or approximant; cf. IPA chart
    # 'ˤ' see below
    'ʢ': 'voiced epiglottal fricative',  # was: or approximant; cf. IPA chart
    'ǀ': 'dental click',
    'ǂ': 'alveolo-palatal click',  # was: palatoalveolar
    'ǁ': 'alveolar lateral click',
    'ǃ': 'postalveolar click',  # was: (post)alveolar; cf. 'ʗ' postalveolar
}

IPA_MODIFIERS = {
    'ᵊ': 'mid central vowel release',
    'ˠ': 'velarized',
    'ʰ': 'aspirated',
    'ʲ': 'palatalized',
    'ˡ': 'lateral release',
    'ⁿ': 'nasal release',
    'ᶿ': 'voiceless dental fricative release',
    'ʷ': 'labialized',
    'ˣ': 'voiceless velar fricative release',
    'ˤ': 'pharyngealized',

    'ʱ': 'breathy voiced aspirated',
    'ʼ': 'ejective',
    'ʽ': 'weakly aspirated',
    'ˀ': "glo'alized",
}

REPLACEMENTS = {
    # Compatibility
    '!': 'ǃ',   # EXCLAMATION MARK: LATIN LETTER RETROFLEX CLICK
    'ƾ': 'ts',  # LATIN LETTER INVERTED GLOTTAL STOP WITH STROKE: t + s
    'ȵ': 'ɲ',   # Curly-tail (alveolo-palatal) N: Left-tail (palatal) N
    'ɿ': 'z̩',   # "apical dental vowel"; [z̩], [ɨ], or [ɯ]
    'ʅ': 'ʐ̩',   # "apical retroflex vowel"; [ʐ̩], [ɨ˞], or [ɨ]
    'γ': 'ɣ',   # Greek gamma: Latin gamma
    'φ': 'ɸ',   # Greek phi: Latin phi
    'ϑ': 'θ',   # Greek theta symbol: Greek theta
    'ѳ': 'θ',   # Cyrillic fita: Greek theta
    'ᴣ': 'ʒ',   # LAT. LETTER SMALL CAPITAL EZH: LATIN SMALL LETTER EZH
    'ᵿ': 'ʉ',   # LAT. SMALL LETTER UPSILON WITH STROKE: LAT. SMALL LETTER U BAR
    'ꞎ': 'ɭ̊˔',  # "voiceless lateral retroflex fricative"
    'ꞵ': 'β',   # Latin beta: Greek beta
    'ꭓ': 'χ',   # Latin chi: Greek chi

    # Latin letters with palatal hook
    'ᶀ': 'bʲ',
    'ꞔ': 'cʲ',
    'ᶁ': 'dʲ',
    'ᶂ': 'fʲ',
    'ᶃ': 'gʲ',
    'ꞕ': 'hʲ',
    'ᶄ': 'kʲ',
    'ᶅ': 'lʲ',
    'ᶆ': 'mʲ',
    'ᶇ': 'nʲ',
    'ᶈ': 'pʲ',
    'ᶉ': 'rʲ',
    'ᶊ': 'sʲ',
    'ᶋ': 'ʃʲ',
    # 'ƫ' see below
    'ᶌ': 'vʲ',
    'ᶍ': 'xʲ',
    'ᶎ': 'zʲ',

    # Latin letters with retroflex hook
    'ᶏ': 'a˞',
    'ᶐ': 'ɑ˞',
    # 'ᶑ' see below
    'ᶒ': 'e˞',
    'ᶓ': 'ɛ˞',
    'ᶔ': 'ɜ˞',
    'ᶕ': 'ə˞',
    'ᶖ': 'i˞',
    'ᶗ': 'ɔ˞',
    'ᶘ': 'ʂ',
    'ᶙ': 'u˞',
    'ᶚ': 'ʐ',

    # Deprecated symbols discussed in the IPA Handbook
    'ƈ': 'ʄ̊',   # Hooktop C: Hooktop barred dotless J + Over-ring
    'č': 'tʃ',  # C wedge (not IPA usage): t + esh
    'ʗ': 'ǃ',   # Stretched C: Exclamation point (LATIN LETTER RETROFLEX CLICK)
    'ʣ': 'dz',  # D-Z ligature: d + z
    'ʤ': 'dʒ',  # D-Ezh ligature: d + ezh
    'ʥ': 'dʑ',  # D-Curly-tail-Z ligature: d + curly-tail z
    'ɚ': 'ə˞',  # Right-hook schwa: schwa + right-hook
    'ʚ': 'ɞ',   # Closed epsilon: Closed reversed epsilon
    'ɝ': 'ɜ˞',  # Right-hook reversed epsilon: reversed epsilon + right-hook
    'ɡ': 'g',   # Opentail or script g: looptail or ASCII g
    'ı': 'ɪ',   # Undotted I (not IPA usage): Small capital I (alt. ɨ or ɯ)
    'ɩ': 'ɪ',   # Iota: Small capital I
    'ǰ': 'dʒ',  # J wedge (not IPA usage): d + ezh
    'ƙ': 'ɠ̊',   # Hooktop K: Hooktop G + Over-ring
    'ʞ': None,  # Turned K (withdrawn 1979)
    'λ': 'ɮ',   # Greek lambda: L-Ezh ligature
    'ƛ': 'tɬ',  # Barred lambda: t + Belted L
    'ƞ': 'n̩',   # Right-leg N: n + syllabicity mark
    'ɷ': 'ʊ',   # Closed omega: Upsilon
    'ƥ': 'ɓ̥',   # Hooktop P: Hooktop B + Under-ring
    'ʠ': 'ʛ̥',   # Hooktop Q: Hooktop small capital G + Under-ring
    'ɼ': 'r̝',   # Long-leg R (withdrawn 1989): r + Raising sign
    'ˢ': None,  # Superscript S (withdrawn 1989)
    'š': 'ʃ',   # S wedge (not IPA usage): Esh
    'ʆ': 'ɕ',   # Curly-tail esh (withdrawn 1989): Curly-tail C
    'ƭ': 'ɗ̥',   # Hooktop T: Hooktop D + Under-ring
    'ƫ': 'tʲ',  # Left-hook T: t + Superscript J
    'ʦ': 'ts',  # T-S ligature: t + s
    'ʧ': 'tʃ',  # T-Esh ligature: t + esh
    'ʨ': 'tɕ',  # T-Curly-tail-C ligature: t + curly-tail c
    'ʇ': 'ǀ',   # Turned T: Pipe (LATIN LETTER DENTAL CLICK)
    'ž': 'ʒ',   # Z wedge (not IPA usage): Ezh
    'ʓ': 'ʑ',  # Curly-taile ezh (withdrawn 1989): Curly-tail z
    'ƻ': 'dz',  # Barred two (withdrawn 1976)
    'ʖ': 'ǁ',   # Inverted glottal stop: Double pipe (LAT. LETTER LATERAL CLICK)
}


CONSONANT_FEATURE = {
    'voiced': 'phonation',
    'voiceless': 'phonation',

    'bilabial': 'place',
    'labiodental': 'place',
    'dental': 'place',
    'dental|alveolar': 'place',
    'alveolar': 'place',
    'postalveolar': 'place',
    'retroflex': 'place',
    'alveolo-palatal': 'place',
    'palatal': 'place',
    'velar': 'place',
    'uvular': 'place',
    'pharyngeal': 'place',
    'epiglottal': 'place',
    'glottal': 'place',

    'lateral': 'centrality',

    'approximant': 'manner',
    'trill': 'manner',
    'flap': 'manner',
    'fricative': 'manner',
    'nasal': 'manner',
    'plosive': 'manner',
    'implosive': 'manner',
    'click': 'manner',
}

VOWEL_FEATURE = {
    'close': 'height',
    'near-close': 'height',
    'close-mid': 'height',
    'mid': 'height',
    'open-mid': 'height',
    'near-open': 'height',
    'open': 'height',

    'front': 'backness',
    'near-front': 'backness',
    'central': 'backness',
    'near-back': 'backness',
    'back': 'backness',

    'unrounded': 'roundedness',
    'rounded': 'roundedness',
}


def CheckDescription(ipa, description, writer):
  """Check that the description of the given IPA symbol is well-formed."""
  result = collections.defaultdict(lambda: [])
  features = description.split()
  assert features
  if features[-1] == 'vowel':
    for f in features[:-1]:
      if f not in VOWEL_FEATURE:
        writer.write('Unknown vowel feature %s in [%s] %s\n' %
                     (f, ipa, description))
      else:
        feature_name = VOWEL_FEATURE[f]
        result[feature_name].append(f)
    assert not any(f in CONSONANT_FEATURE for f in features)
  else:
    for f in features:
      if f not in CONSONANT_FEATURE:
        writer.write('Unknown consonant feature %s in [%s] %s\n' %
                     (f, ipa, description))
      else:
        feature_name = CONSONANT_FEATURE[f]
        result[feature_name].append(f)
    assert not any(f in VOWEL_FEATURE for f in features)
  return result


def CheckDescriptions(writer):
  for ipa, description in IPA_LETTERS.items():
    features = CheckDescription(ipa, description, writer)
    for feature_name, values in features.items():
      assert values
      if len(values) > 1:
        assert feature_name == 'place'
        assert len(values) == 2
  return


def PrintCharTable(writer):
  """Prints a table of known phonetic symbols."""
  letters = frozenset(IPA_LETTERS)
  modifiers = frozenset(IPA_MODIFIERS)
  replacements = frozenset(REPLACEMENTS)
  assert not letters & modifiers
  assert not letters & replacements
  assert not modifiers & replacements
  ipa = IPA_LETTERS.copy()
  ipa.update(IPA_MODIFIERS)
  for char in sorted(letters | modifiers | replacements):
    if char in replacements:
      writer.write('%04X\t%s\t→ %s\n' % (ord(char), char, REPLACEMENTS[char]))
    else:
      writer.write('%04X\t%s\t%s\n' % (ord(char), char, ipa[char]))
  return


if __name__ == '__main__':
  stdout = io.open(1, mode='wt', encoding='utf-8', closefd=False)
  CheckDescriptions(stdout)
  PrintCharTable(stdout)

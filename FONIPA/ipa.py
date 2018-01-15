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

import io
import re
import unicodedata

# Base letters from the IPA Handbook, mostly Table 3
BASE_LETTERS = {
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
    'ɡ': 'voiced velar plosive',  # IPA Number 110
    'ɠ': 'voiced velar implosive',
    'g': 'voiced velar plosive',  # IPA Number 210
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
    # was: simultaneous voiceless postalveolar and velar fricative
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
    'ⱱ': 'voiced labiodental flap',
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
    'ʔ': 'voiceless glottal plosive',     # added: voiceless as a default
    'ʡ': 'voiceless epiglottal plosive',  # added: voiceless as a default
    'ʕ': 'voiced pharyngeal fricative',  # was: or approximant; cf. IPA chart
    # 'ˤ' see below
    'ʢ': 'voiced epiglottal fricative',  # was: or approximant; cf. IPA chart
    'ǀ': 'dental click',
    'ǂ': 'palatal click',  # was: palatoalveolar
    'ǁ': 'alveolar lateral click',
    'ǃ': 'alveolar click',  # was: (post)alveolar
}

# Modifier letters and symbols from the IPA Handbook
MODIFIER_LETTERS = {
    # 400-series from Table 4
    'ʼ': 'ejective',
    'ʰ': 'aspirated',
    '˞': 'rhoticity',
    'ʷ': 'labialized',
    'ʲ': 'palatalized',
    'ˠ': 'velarized',
    'ˤ': 'pharyngealized',
    'ⁿ': 'nasal release',
    'ˡ': 'lateral release',
    '˔': 'raised',
    '˕': 'lowered',

    'ˈ': 'primary stress',
    'ˌ': 'secondary stress',
    'ː': 'long',
    'ˑ': 'half-long',
    '.': 'syllable break',
    '|': 'minor group',
    '‖': 'major group',
    '‿': 'linking',

    '˥': 'extra high level',
    '˦': 'high level',
    '˧': 'mid level',
    '˨': 'low level',
    '˩': 'extra low level',
    # Contours (IPA 529-533) can be formed by combinations of tone letters.
    '↓': 'downstep',
    '↑': 'upstep',
    '↗': 'global rise',
    '↘': 'global fall',

    # 200-series from Table 3
    'ᵊ': 'mid central vowel release',
    'ᶿ': 'voiceless dental fricative release',
    'ˣ': 'voiceless velar fricative release',

    # Widely used, but not mentioned in the IPA Handbook
    'ʱ': 'breathy voiced aspirated',
    'ˀ': 'glottalized',
    'ᵐ': 'bilabial prenasalization',
    'ᵑ': 'velar prenasalization',
}

# Combining marks from the IPA Handbook, Table 4
COMBINING_MARKS = {
    '\u0325': 'voiceless',
    '\u030A': 'voiceless',
    '\u032C': 'voiced',
    '\u0339': 'more rounded',
    '\u031C': 'less rounded',
    '\u031F': 'advanced',
    '\u0320': 'retracted',
    '\u0308': 'centralized',
    '\u0338': 'mid-centralized',
    '\u0329': 'syllabic',
    '\u032F': 'non-syllabic',
    '\u0324': 'breathy voiced',
    '\u0330': 'creaky voiced',
    '\u033C': 'linguolabial',
    '\u032A': 'dental',
    '\u033A': 'apical',
    '\u033B': 'laminal',
    '\u0303': 'nasalized',
    '\u031A': 'no audible release',
    '\u0334': 'velarized or pharyngealized',
    '\u031D': 'raised',
    '\u031E': 'lowered',
    '\u0318': 'advanced tongue root',
    '\u0319': 'retracted tongue root',
    '\u0361': 'affricate or double articulation',
    '\u035C': 'affricate or double articulation',  # per the IPA chart

    '\u0306': 'extra-short',

    '\u030B': 'extra high level',
    '\u0301': 'high level',
    '\u0304': 'mid level',
    '\u0300': 'low level',
    '\u030F': 'extra low level',
    '\u030C': 'rising contour',
    '\u0302': 'falling contour',
    '\u1DC4': 'high rising contour',
    '\u1DC5': 'low rising contour',
    '\u1DC8': 'rising-falling contour',
}

# It is often helpful to decompose a Unicode string holding IPA transcriptions
# before comparing it against the tables above. Many letter+diacritic
# combinations used in IPA transcription can be encoded as individual Unicode
# characters or as a combination of a base character and a combining mark. For
# example [ṍ] (close-mid back rounded nasal vowel in high level tone) can be
# represented as any of the following Unicode character strings:
#
#   1E4D
#   00F5 0301
#   006F 0303 0301
#
# The last representation is often convenient for analyzing the meaning of the
# composed notation symbol, however encoded, since all components can be
# easily looked up in the tables above. This generally corresponds to Unicode
# Normalization Form D (NFD), with the exception of c-cedilla [ç], which
# decomposes to <c 0327>. In IPA [ç] is a voiceless palatal fricative and [c]
# is a voiceless palatal plosive, but 0327 is not intended to be a fortition
# diacritic that would turn a fricative into a corresponding plosive. Instead
# [ç] should be treated atomically.
#
# The function Decompose() defined below is intended as a solution. It first
# decomposes its argument into NFD form, then combines all occurrences of 'c'
# and 0327 within the same grapheme cluster into 'ç'.

class Rewrite(object):

  def __init__(self, pattern, replacement):
    self.pattern = pattern
    self.replacement = replacement
    return

  def rewrite(self, string):
    return re.sub(self.pattern, self.replacement, string)


C_CEDILLA = Rewrite(
    re.compile(r'c([\u0300-\u036F\u1AB0-\u1AFF\u1DC0-\u1DFF]*)\u0327'),
    r'ç\1')

NONRHOTIC = {'ɚ': 'ə', 'ɝ': 'ɜ'}

RHOTIC_VOWEL = Rewrite(
    re.compile(r'([ɚɝ])([\u0300-\u036F\u1AB0-\u1AFF\u1DC0-\u1DFF]*)'),
    lambda m: '%s%s˞' % (NONRHOTIC.get(m.group(1)), m.group(2)))


def Decompose(string):
  r"""Convert an IPA string into a convenient decomposed form.

  Essentially this means conversion to Unicode NFD form. However, there
  are a few differences:

  First, c-cedilla 'ç' is not decomposed into its NFD form but is left
  intact.

  Second, character combinations that the IPA Handbook designates as
  equivalent have identical decompositions. In particular Opentail G
  (IPA Number 110) and Looptail G (IPA Number 220) decompose
  identically; and Right-hook schwa (IPA Number 327) decomposes into its
  equivalent representation Schwa + Right hook (IPA Numbers 322 + 419).

  Args:
    string: string to be decomposed

  Returns:
    the decomposed string

  For example '\u0107\u0327' ('ḉ') decomposes to '\u00E7\u0301' ('ḉ'):
  >>> Decompose('\u0107\u0327') == '\u00E7\u0301'
  True
  >>> Decompose('\u0261') == Decompose('g')
  True
  >>> Decompose('\u0261\u0306') == Decompose('\u011F') == 'g\u0306'
  True
  >>> Decompose('\u025A') == Decompose('\u0259\u02DE')
  True
  >>> Decompose('\u025A\u0301') == '\u0259\u0301\u02DE'
  True
  >>> Decompose('\u025D\u0303\u0330') == '\u025C\u0330\u0303\u02DE'
  True
  """
  string = RHOTIC_VOWEL.rewrite(string)
  string = unicodedata.normalize('NFD', string)
  string = string.replace('ɡ', 'g')
  string = C_CEDILLA.rewrite(string)
  return string


REPLACEMENTS = {
    # Compatibility
    '!': 'ǃ',   # EXCLAMATION MARK: LATIN LETTER RETROFLEX CLICK
    "'": 'ʼ',   # APOSTROPHE: MODIFIER LETTER APOSTROPHE
    ':': 'ː',   # COLON: MODIFIER LETTER TRIANGULAR COLON
    'ƒ': 'ʄ',   # F WITH HOOK: DOTLESS J WITH STROKE AND HOOK
    'ƾ': 'ts',  # LATIN LETTER INVERTED GLOTTAL STOP WITH STROKE: t + s
    'ȵ': 'ɲ',   # Curly-tail (alveolo-palatal) N: Left-tail (palatal) N
    'ɿ': 'z̩',   # "apical dental vowel"; [z̩], [ɨ], or [ɯ]
    'ʅ': 'ʐ̩',   # "apical retroflex vowel"; [ʐ̩], [ɨ˞], or [ɨ]
    'ˁ': 'ˤ',   # REVERSED GLOTTAL STOP: SMALL REVERSED GLOTTAL STOP
    'γ': 'ɣ',   # Greek gamma: Latin gamma
    'ε': 'ɛ',   # Greek epsilon: Latin epsilon
    'φ': 'ɸ',   # Greek phi: Latin phi
    'ϐ': 'β',   # Greek beta symbol: Greek beta
    'ϑ': 'θ',   # Greek theta symbol: Greek theta
    'ѳ': 'θ',   # Cyrillic fita: Greek theta
    'ә': 'ə',   # Cyrillic schwa: Latin schwa
    'ᴈ': 'ɜ',   # TURNED OPEN E: REVERSED OPEN E
    'ᴐ': 'ɔ',   # SMALL CAPITAL OPEN O: SMALL LETTER OPEN O
    'ᴚ': 'ʁ',   # SMALL CAPITAL TURNED R: SMALL CAPITAL INVERTED R
    'ᴣ': 'ʒ',   # SMALL CAPITAL EZH: SMALL LETTER EZH
    'ᵿ': 'ʉ',   # UPSILON WITH STROKE: U BAR
    'ⱴ': 'ⱱ',   # V WITH CURL: V WITH RIGHT HOOK
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

    # Deprecated letters discussed in the IPA Handbook
    'ƈ': 'ʄ̊',   # Hooktop C: Hooktop barred dotless J + Over-ring
    'č': 'tʃ',  # C wedge (not IPA usage): t + esh
    'ʗ': 'ǃ',   # Stretched C: Exclamation point (LATIN LETTER RETROFLEX CLICK)
    'ʣ': 'dz',  # D-Z ligature: d + z
    'ʤ': 'dʒ',  # D-Ezh ligature: d + ezh
    'ʥ': 'dʑ',  # D-Curly-tail-Z ligature: d + curly-tail z
    'ɚ': 'ə˞',  # Right-hook schwa: schwa + right-hook
    'ʚ': 'ɞ',   # Closed epsilon: Closed reversed epsilon
    'ɝ': 'ɜ˞',  # Right-hook reversed epsilon: reversed epsilon + right-hook
    'ı': 'ɪ',   # Undotted I (not IPA usage): Small capital I (alt. ɨ or ɯ)
    'ɩ': 'ɪ',   # Iota: Small capital I
    'ǰ': 'dʒ',  # J wedge (not IPA usage): d + ezh
    'ƙ': 'ɠ̊',   # Hooktop K: Hooktop G + Over-ring
    'ʞ': None,  # Turned K (withdrawn 1979)
    'λ': 'ɬ',   # Greek lambda: Belted L
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
    'ʓ': 'ʑ',   # Curly-tail ezh (withdrawn 1989): Curly-tail z
    'ƻ': 'dz',  # Barred two (withdrawn 1976)
    'ʖ': 'ǁ',   # Inverted glottal stop: Double pipe (LAT. LETTER LATERAL CLICK)

    # Deprecated modifiers and combining marks discussed in the IPA Handbook
    '\u0322': '˞',       # Subscript right hook: Right hook
    ',': None,           # Comma (not IPA usage)
    'ʻ': None,           # Reversed apostrophe (withdrawn 1979)
    '\u0307': None,      # Over-dot (withdrawn 1979)
    '˗': '\u0320',       # Minus sign
    '˖': '\u031F',       # Plus sign
    'ʸ': None,           # Superscript Y (not IPA usage)
    '\u0323': '\u031D',  # Under-dot: Raising sign
    '\u0321': 'ʲ',       # Subscript left hook: Superscript J
    '\u032B': 'ʷ',       # Subscript W: Superscript W
    '\u0311': None,      # Superscript arch (not IPA usage)
    'ˇ': '˧˨˧',          # Wedge (usage redefined): falling-rising contour
    'ˆ': '˧˦˧',          # Circumflex (usage redefined): rising-falling contour
    'ˎ': '˧˩',           # Subscript grave accent: low falling contour
    '\u0316': '˧˩',      # Combining subscript grave accent: ditto
    'ˏ': '˩˧',           # Subscript acute accent: low rising contour
    '\u0317': '˩˧'       # Combining subscript acute accent: ditto
}

CONSONANT_FEATURE = {
    'voiced': 'Voicing',
    'voiceless': 'Voicing',

    'bilabial': 'Place',
    'labiodental': 'Place',
    'dental': 'Place',
    'dental|alveolar': 'Place',
    'alveolar': 'Place',
    'postalveolar': 'Place',
    'retroflex': 'Place',
    'alveolo-palatal': 'Place',
    'palatal': 'Place',
    'velar': 'Place',
    'uvular': 'Place',
    'pharyngeal': 'Place',
    'epiglottal': 'Place',
    'glottal': 'Place',

    'lateral': 'Centrality',

    'approximant': 'Manner',
    'trill': 'Manner',
    'flap': 'Manner',
    'fricative': 'Manner',
    'nasal': 'Manner',
    'plosive': 'Manner',
    'implosive': 'Manner',
    'click': 'Manner',
}

VOWEL_FEATURE = {
    'close': 'Height',
    'near-close': 'Height',
    'close-mid': 'Height',
    'mid': 'Height',
    'open-mid': 'Height',
    'near-open': 'Height',
    'open': 'Height',

    'front': 'Backness',
    'near-front': 'Backness',
    'central': 'Backness',
    'near-back': 'Backness',
    'back': 'Backness',

    'unrounded': 'Roundedness',
    'rounded': 'Roundedness',
}


def CheckDescription(ipa, description):
  """Check that the description of the given IPA symbol is well-formed."""
  result = {}
  terms = description.split()
  assert terms
  if terms[-1] == 'vowel':
    assert not any(t in CONSONANT_FEATURE for t in terms)
    for term in terms[:-1]:
      assert term in VOWEL_FEATURE, (
          'Unknown term %s describing vowel [%s] %s\n' %
          (term, ipa, description))
      feature_name = VOWEL_FEATURE[term]
      assert feature_name not in result
      result[feature_name] = term
  else:
    assert not any(t in VOWEL_FEATURE for t in terms)
    for term in terms:
      assert term in CONSONANT_FEATURE, (
          'Unknown term %s describing consonant [%s] %s\n' %
          (term, ipa, description))
      feature_name = CONSONANT_FEATURE[term]
      if feature_name == 'Place':
        places = result.get(feature_name, [])
        places.append(term)
        result[feature_name] = places
      else:
        assert feature_name not in result
        result[feature_name] = term
    places = result.get('Place', [])
    assert 1 <= len(places) <= 2
  return result


def IntegrityChecks():
  """Perform basic data integrity checks on the embedded tables."""
  for char, description in BASE_LETTERS.items():
    assert len(char) == 1
    assert unicodedata.category(char) in ('Ll', 'Lo')
    CheckDescription(char, description)
  ancient = unicodedata.unidata_version.split('.')[:2] <= ['5', '1']
  for char in MODIFIER_LETTERS:
    assert len(char) == 1
    if ancient and char == 'ⁿ':
      # Unicode 5.1 has a category inconsistency for certain superscript
      # letters, categorizing them as Ll instead of Lm.
      continue
    assert unicodedata.category(char) in ('Lm', 'Sk', 'Sm', 'So', 'Pc', 'Po')
  for char in COMBINING_MARKS:
    assert len(char) == 1
    assert unicodedata.category(char) == 'Mn'
  chars = set()
  for table in (BASE_LETTERS, MODIFIER_LETTERS, COMBINING_MARKS, REPLACEMENTS):
    c = frozenset(table.keys())
    assert not chars & c
    chars.update(c)
  return


def PrintCharTable(writer):
  """Print a table of known phonetic symbols."""
  ipa = BASE_LETTERS.copy()
  ipa.update(MODIFIER_LETTERS)
  ipa.update(COMBINING_MARKS)
  chars = set(ipa)
  chars.update(REPLACEMENTS)
  for char in sorted(chars):
    if char in REPLACEMENTS:
      writer.write('%04X\t%s\t→ %s\n' % (ord(char), char, REPLACEMENTS[char]))
    else:
      writer.write('%04X\t%s\t%s\n' % (ord(char), char, ipa[char]))
  return


def PrintChart(writer):
  """Pretty-print the major elements of the IPA chart."""
  description_to_letter = {}
  for letter, description in BASE_LETTERS.items():
    if letter == 'ɡ':
      continue
    assert description not in description_to_letter
    description_to_letter[description] = letter

  places = [
      'bilabial', 'labiodental', 'dental', 'dental|alveolar', 'alveolar',
      'postalveolar', 'retroflex', 'palatal', 'velar', 'uvular', 'pharyngeal',
      # 'epiglottal',
      'glottal',
  ]
  writer.write('┌%s┐\n' % '┬'.join(['─────'] * (len(places) - 1)))
  for manner in ('plosive', 'nasal', 'trill', 'flap', 'fricative',
                 'lateral fricative', 'approximant', 'lateral approximant'):
    row = []
    for place in places:
      cell = []
      for voicing in ('voiceless', 'voiced'):
        description = '%s %s %s' % (voicing, place, manner)
        cell.append(description_to_letter.get(description, ' '))
      row.append(' '.join(cell))
    if manner != 'fricative':
      assert not row[2].strip()
      assert row[3].strip()
      assert not row[4].strip()
      assert not row[5].strip()
      del row[4]
    else:
      assert row[2].strip()
      assert not row[3].strip()
      assert row[4].strip()
      assert row[5].strip()
      del row[3]
    writer.write('│ %s │\n' % ' │ '.join(row))
  writer.write('└%s┘\n\n' % '┴'.join(['─────'] * (len(places) - 1)))

  grid = [
      ' •───────────•─────• ',
      '  ╲          │     │ ',
      '   •─────────•─────• ',
      '    ╲        │     │ ',
      '     •───────•─────• ',
      '      ╲      │     │ ',
      '       •─────┴─────• ',
  ]
  for i, height in enumerate(('close', 'near-close', 'close-mid', 'mid',
                              'open-mid', 'near-open', 'open')):
    row = [c for c in grid[i]]
    for j, backness in enumerate(('front', 'near-front', 'central',
                                  'near-back', 'back')):
      for k, rvowel in enumerate(('unrounded vowel', 'vowel', 'rounded vowel')):
        description = '%s %s %s' % (height, backness, rvowel)
        letter = description_to_letter.get(description, '')
        if letter:
          pos = (i if j < 2 else 6) + 3 * j + k
          row[pos] = letter
    writer.write('%s\n' % ''.join(row))
  return


IntegrityChecks()

if __name__ == '__main__':
  stdout = io.open(1, mode='wt', encoding='utf-8', closefd=False)
  # PrintCharTable(stdout)
  PrintChart(stdout)

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

"""Evaluate pronunciation rules against the NCHLT dictionaries.
"""

from __future__ import unicode_literals

import icu  # Debian/Ubuntu: apt-get install python-pyicu python3-icu
import sys

from utils import utf8

EXCEPTIONAL_WORDS = frozenset([
    'apreli',  # known exception in the NCHLT isiXhosa dictionary
    ])


def GetPronunciationRules(path, name):
  with utf8.open(path) as reader:
    rules = reader.read()
  return icu.Transliterator.createFromRules(
      name, rules, icu.UTransDirection.FORWARD)


def GetSampaToIpaMapping(path):
  mapping = {}
  with utf8.open(path) as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) == 2
      sampa, ipa = fields
      assert sampa not in mapping
      mapping[sampa] = ipa
  return mapping


def TestPronunciationRules(xltor, mapping, dictionary):
  # Batch testing against a dictionary.
  success = True
  with utf8.open(dictionary) as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) == 2
      orth, pron = fields
      sampa = pron.split()
      ipa = ''.join(mapping[p] for p in sampa)
      if orth in EXCEPTIONAL_WORDS:
        continue
      predicted = xltor.transliterate(orth)
      if predicted != ipa:
        utf8.Print('%s\t%s\t%s != %s' %
                   (orth, ' '.join(sampa), ipa, predicted))
        success = False
  return success


def ApplyPronunciationRules(xltor):
  # For interactive use.
  for line in utf8.stdin:
    for orth in line.split():
      predicted = xltor.transliterate(orth)
      utf8.Print('%s\t%s' % (orth, predicted))
  return


def main(args):
  if len(args) == 2:
    xltor = GetPronunciationRules(args[1], 'foo-bar')
    ApplyPronunciationRules(xltor)
  elif len(args) == 4:
    xltor = GetPronunciationRules(args[1], 'foo-bar')
    mapping = GetSampaToIpaMapping(args[2])
    if TestPronunciationRules(xltor, mapping, args[3]):
      utf8.Print('PASS')
      sys.exit(0)
    else:
      utf8.Print('FAIL')
      sys.exit(1)
  else:
    utf8.Print('Usage: %s RULES [MAPPING DICTIONARY]' % args[0])
    sys.exit(2)
  return


if __name__ == '__main__':
  main(sys.argv)

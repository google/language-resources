#! /usr/bin/python2 -u
# -*- coding: utf-8 -*-
#
# Copyright 2016 Google Inc. All Rights Reserved.
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

"""Tokenize isiXhosa words according to word-internal capitalization.
"""

from __future__ import unicode_literals

import codecs
import icu  # Debian/Ubuntu: apt-get install python-pyicu
import sys

STDOUT = codecs.getwriter('utf-8')(sys.stdout)


def GetPronunciationRules(path, name):
  rules = codecs.open(path, 'r', 'utf-8').read()
  return icu.Transliterator.createFromRules(
      name, rules, icu.UTransDirection.FORWARD)


def GetSampaToIpaMapping(path):
  mapping = {}
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) == 2
      sampa, ipa = fields
      assert sampa not in mapping
      mapping[sampa] = ipa
  return mapping


def ReadPronunciationDictionary(f, mapping):
  while True:
    line = f.readline()
    if not line:
      break
    line = line.decode('utf-8')
    line = line.rstrip('\n')
    fields = line.split('\t')
    assert len(fields) == 2
    orth, pron = fields
    sampa = pron.split()
    ipa = ''.join(mapping[p] for p in sampa)
    yield orth, sampa, ipa
  return


if __name__ == '__main__':
  assert len(sys.argv) == 4
  argv0, rules, phonemes, dictionary = sys.argv

  xltor = GetPronunciationRules(rules, 'xh-xh_FONIPA')
  mapping = GetSampaToIpaMapping(phonemes)
  with open(dictionary, 'r') as f:
    for orth, sampa, ipa in ReadPronunciationDictionary(f, mapping):
      if orth == 'apreli':  # known issue
        continue
      expected = xltor.transliterate(orth)
      if expected != ipa:
        STDOUT.write('%s\t%s\t%s != %s\n' %
                     (orth, ' '.join(sampa), ipa, expected))
        sys.exit(1)

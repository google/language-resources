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

"""Evaluate isiXhosa pronunciation rules against the NCHLT dictionary.
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


def TestPronunciationRules(xltor, mapping, dictionary):
  # Batch testing against a dictionary.
  with codecs.open(dictionary, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      assert len(fields) == 2
      orth, pron = fields
      sampa = pron.split()
      ipa = ''.join(mapping[p] for p in sampa)
      if orth == 'apreli':  # known issue
        continue
      predicted = xltor.transliterate(orth)
      if predicted != ipa:
        STDOUT.write('%s\t%s\t%s != %s\n' %
                     (orth, ' '.join(sampa), ipa, predicted))
        sys.exit(1)
  return


def ApplyPronunciationRules(xltor, mapping):
  # For interactive use.
  while True:
    line = sys.stdin.readline()
    if not line:
      break
    line = line.decode('utf-8')
    for orth in line.split():
      predicted = xltor.transliterate(orth)
      STDOUT.write('%s\t%s\n' % (orth, predicted))
  return


if __name__ == '__main__':
  assert 3 <= len(sys.argv) <= 4
  xltor = GetPronunciationRules(sys.argv[1], 'xh-xh_FONIPA')
  mapping = GetSampaToIpaMapping(sys.argv[2])
  if len(sys.argv) > 3:
    TestPronunciationRules(xltor, mapping, sys.argv[3])
  else:
    ApplyPronunciationRules(xltor, mapping)

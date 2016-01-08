#! /usr/bin/python2
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

"""Test general ICU transforms against a dictionary.
"""

from __future__ import unicode_literals

import codecs
import icu  # Debian/Ubuntu: apt-get install python-pyicu
import sys

STDOUT = codecs.getwriter('utf-8')(sys.stdout)

if len(sys.argv) != 3:
  STDOUT.write('Usage: %s RULES DICTIONARY\n' % sys.argv[0])
  sys.exit(1)

rules = codecs.open(sys.argv[1], 'r', 'utf-8').read()
xltor = icu.Transliterator.createFromRules(
    'foo-bar', rules, icu.UTransDirection.FORWARD)

success = True
for line in codecs.open(sys.argv[2], 'r', 'utf-8'):
  fields = line.rstrip('\n').split('\t')
  orth, pron = fields
  predicted = xltor.transliterate(orth)
  if predicted != pron:
    STDOUT.write('%s\t%s != %s\n' % (orth, pron, predicted))
    success = False

if success:
  STDOUT.write('PASS\n')
  sys.exit(0)
else:
  STDOUT.write('FAIL\n')
  sys.exit(1)

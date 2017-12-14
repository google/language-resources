#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 Google Inc. All Rights Reserved.
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

"""Transforms recording script from IIIT-H Bengali Speech Database.

Reads data from the IIIT-H Bengali Festvox database and transforms
it into TSV format. Applies corrections along the way.

The output is a 3-column TSV (tab-separated values) file with the following
columns:

  1. Utterance ID
  2. Bengali text
  3. Transliterated text

If a line in the output starts with '#', there is potentially a problem with
the utterance. We flag suspicious utterances where the number of tokens in
the Bengali text does not match the number of tokens in the transliteration.

Usage example:
  $ ./munge_iiit_ben.py | cut -f2 | tr -s ' ' '\n' | sort | uniq -c | sort -n
"""

import codecs
import os.path
import re
import sys
import unicodedata

DATA_RE = re.compile(r'\( ben_([0-9]{4,4}) " ([^ ].*[^ ]) *"\)\s*$')
CLEANUP_RE = re.compile(r'[ :]+')

# Suspicious utterances, transcript should be compared to wave file:
UTTERANCE_BLACKLIST = set(['0045', '0046', '0060', '0246'])

STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)


def ReadData(path):
  """Reads data from a txt.done.data* file."""
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      match = DATA_RE.match(line)
      assert match is not None
      yield match.group(1), match.group(2)
  return


def GetUtterances(etc_dir):
  """Iterates over the utterances in the Bengali recording script."""
  beng_data = list(ReadData(os.path.join(etc_dir, 'txt.done.data.utf8')))
  xlit_data = list(ReadData(os.path.join(etc_dir, 'txt.done.data')))
  assert len(beng_data) == len(xlit_data)
  for (id1, beng), (id2, xlit) in zip(beng_data, xlit_data):
    assert id1 == id2
    if id1 == '0060':
      assert u'π' in beng
      beng = beng.replace(u'π', u'পাই')
    beng = CLEANUP_RE.sub(' ', beng).strip()
    xlit = xlit.replace('uikiped:iyaabaan:laa', 'uikiped:iyaa baan:laa')
    assert unicodedata.normalize('NFC', beng) == beng
    if id1 in UTTERANCE_BLACKLIST:
      id1 = '#' + id1
    else:
      assert len(beng.split()) == len(xlit.split())
    yield id1, beng, xlit
  return


if __name__ == '__main__':
  etc_dir = os.path.join(os.path.dirname(sys.argv[0]),
                         '../third_party/iiit_ben_ant/etc/')
  for fields in GetUtterances(etc_dir):
    STDOUT.write('%s\n' % '\t'.join(fields))

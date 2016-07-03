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

"""Extract Myanmar codepoint bigrams from text.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

import extract_text

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

IGNORE = re.compile(r'[-()]+')


def MyanmarPhrases(reader):
  for phrase in extract_text.ExtractText(reader):
    yield IGNORE.sub('', phrase)
  return


def NGrams(phrases, n, separator):
  assert n >= 1
  history = [separator]
  while len(history) >= n:
    history.pop(0)
  for phrase in phrases:
    for c in phrase:
      yield c, history
      history.append(c)
      while len(history) >= n:
        history.pop(0)
    yield separator, history
    history.append(separator)
    while len(history) >= n:
      history.pop(0)
  return


def FormatNGrams(ngrams):
  for current, history in ngrams:
    yield ' '.join('%04X' % ord(cp) for cp in history + [current])
  return


def Count(ngrams):
  total = 0
  count = {}
  for ngram in ngrams:
    total += 1
    count[ngram] = 1 + count.get(ngram, 0)
  return total, count


def PrintNgramCounts(n=2):
  ngrams = NGrams(MyanmarPhrases(STDIN), n, ' ')
  total, count = Count(FormatNGrams(ngrams))
  items = count.items()
  items.sort(key=lambda (ngrm, cnt): (-cnt, ngrm))
  for ngrm_cnt in items:
    STDOUT.write('%s\t%d\n' % ngrm_cnt)
  return


def main(argv):
  PrintNgramCounts()
  return


if __name__ == '__main__':
  main(sys.argv)

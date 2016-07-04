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

Usage examples:

* Print counts of codepoint bigrams in Myanmar text:
  $ ./bigrams.py < some_file.txt

* Print count of unseen codepoint bigrams for each file in a directory:
  $ find path/to/directory -type f | ./bigrams.py known_bigrams.txt

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


def ReadNgrams(path, default_count=1):
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      fields = line.split('\t')
      if len(fields) >= 2:
        yield fields[0], int(fields[1])
      else:
        yield fields[0], default_count
  return


def ProcessFile(known_bigrams, path):
  total = 0
  unseen = 0
  with codecs.open(path, 'r', 'utf-8') as reader:
    ngrams = FormatNGrams(NGrams(MyanmarPhrases(reader), 2, ' '))
    for ngram in ngrams:
      total += 1
      if ngram not in known_bigrams:
        unseen += 1
  sys.stdout.write('%s\t%d\t%d\t' % (path, unseen, total))
  if total == 0:
    sys.stdout.write('NaN\n')
  else:
    sys.stdout.write('%f\n' % (unseen * 100.0 / total))
  return


def ProcessFiles(known_bigrams):
  for path in sys.stdin:
    path = path[:-1]
    ProcessFile(known_bigrams, path)
  return


def main(argv):
  if len(argv) == 1:
    PrintNgramCounts()
  elif len(argv) == 2:
    known_bigrams = dict(ReadNgrams(argv[1]))
    ProcessFiles(known_bigrams)
  else:
    STDERR.write('Usage: %s [frequent_bigrams.txt]\n' % argv[0])
    sys.exit(2)
  return


if __name__ == '__main__':
  main(sys.argv)

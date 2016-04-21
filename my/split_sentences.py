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

"""Splits the first column of a TSV file into smaller units.

Reads a TSV file from stdin, considers the first column to contain a sentence,
splits that on whitespace, and outputs it in smaller chunks. Chunk length is
counted in Unicode codepoints, not visual grapheme clusters.

"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def SplitSentence(writer, sentence, maxlen):
  line_length = 0
  for phrase in sentence.split():
    if line_length > 0:
      writer.write(' ')
      line_length += 1
    writer.write(phrase)
    line_length += len(phrase)
    if line_length > maxlen:
      writer.write('\n')
      line_length = 0
  if line_length > 0:
    writer.write('\n')
  return


def main(argv):
  if len(argv) > 1:
    maxlen = int(argv[1])
  else:
    maxlen = 75
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    fields = line.split('\t')
    if not fields:
      continue
    SplitSentence(STDOUT, fields[0], maxlen)
    STDOUT.write('\n')
  return


if __name__ == '__main__':
  main(sys.argv)

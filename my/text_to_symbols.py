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

"""Convert text to symbolic codepoint sequence.
"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def TextToSymbols(text):
  for ch in text:
    if ch in '\u200B\u200C\u200D':
      continue
    if ch in '()':
      yield ch
      continue
    cp = ord(ch)
    if 0xAA60 <= cp <= 0xAA7F:
      yield 'AA60-AA7F'
      continue
    elif 0xA9E0 <= cp <= 0xA9FF:
      yield 'A9E0-A9FF'
      continue
    assert 0x1000 <= cp <= 0x109F
    if cp in (0x101D, 0x1040):
      yield '101D|1040'
    elif cp in (0x1044, 0x104E):
      yield '1044|104E'
    else:
      yield '%04X' % cp
  return


def main(unused_argv):
  for line in STDIN:
    line = line.rstrip('\n')
    STDOUT.write('%s\n' % ' '.join(TextToSymbols(line)))
  return


if __name__ == '__main__':
  main(sys.argv)

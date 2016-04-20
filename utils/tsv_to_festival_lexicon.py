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

"""Converts a pronunciation lexicon in TSV format to Festival lexicon format.
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


def main(unused_args):
  STDOUT.write('MNCL\n')
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    fields = line.split('\t')
    assert len(fields) >= 2
    orth = fields[0]
    sylls = ' '.join('((%s) 1)' % s for s in fields[1].split(' . '))
    STDOUT.write('("%s" nil (%s))\n' % (orth, sylls))
  return


if __name__ == '__main__':
  main(sys.argv)

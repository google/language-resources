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

"""Extract contiguous ranges of Myanmar codepoints.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

ENTITY_10 = re.compile(r'&#([0-9]+);')
ENTITY_16 = re.compile(r'&#[xX]([0-9A-Fa-f]+);')

# Contiguous ranges of text in Myanmar codeblocks.
MYANMAR_RE = re.compile(r'''
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF()\u200B-\u200D-]*
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF]+
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF()\u200B-\u200D-]*
''', re.VERBOSE)

ZERO_WIDTH = re.compile(r'[\u200B-\u200D]+')


def UnescapeEntities(text):
  text = ENTITY_10.sub(lambda m: unichr(int(m.group(1), 10)), text)
  text = ENTITY_16.sub(lambda m: unichr(int(m.group(1), 16)), text)
  return text


def Split(line):
  end = 0
  for match in MYANMAR_RE.finditer(line):
    if match.start() != end:
      unmatched = line[end:match.start()]
      yield False, unmatched
    yield True, match.group(0)
    end = match.end()
  if end < len(line):
    yield False, line[end:]
  return


def ExtractText(iterable, debug=None):
  for line in iterable:
    line = line.rstrip('\n')
    if debug:
      debug.write('%s\n' % line)
    line = UnescapeEntities(line)
    for myanmar, text in Split(line):
      if myanmar:
        if debug:
          debug.write('   matched: %s\n' % text)
        else:
          text = ZERO_WIDTH.sub('', text)
          yield text
      elif debug:
        debug.write(' Unmatched: %s\n' % repr(text))
  return


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def main(argv):
  if len(argv) > 1 and argv[1].endswith('debug'):
    for _ in ExtractText(GetlineUnbuffered(), STDOUT):
      pass
  else:
    for text in ExtractText(STDIN):
      STDOUT.write('%s\n' % text)
  return


if __name__ == '__main__':
  main(sys.argv)

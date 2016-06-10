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

"""Extract contiguous ranges of Myanmar codepoints.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

# Contiguous ranges of text in Myanmar codeblocks.
MYANMAR_RE = re.compile(r'''
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF()\u200B-\u200D-]*
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF]+
[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF()\u200B-\u200D-]*
''', re.VERBOSE)


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


def Debug():
  for line in STDIN:
    STDOUT.write('\n%s\n' % line)
    line = line.rstrip('\n')
    for myanmar, text in Split(line):
      if myanmar:
        STDOUT.write('  matched: %s\n' % text)
      else:
        STDOUT.write('Unmatched: %s\n' % repr(text))
  return


def main():
  for line in STDIN:
    line = line.rstrip('\n')
    for myanmar, text in Split(line):
      if myanmar:
        STDOUT.write('%s\n' % text)
  return


if __name__ == '__main__':
  if len(sys.argv) > 1 and sys.argv[1].endswith('debug'):
    Debug()
  else:
    main()

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

"""Count occurrences of codepoints in Myanmar Unicode blocks.
"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def main(unused_argv):
  count = {}
  for line in STDIN:
    for ch in line:
      cp = ord(ch)
      if (0x1000 <= cp <= 0x109F or 0x200B <= cp <= 0x200D or
          0xA9E0 <= cp <= 0xA9FF or 0xAA60 <= cp <= 0xAA7F):
        count[cp] = count.get(cp, 0) + 1
  for cp in sorted(count.keys()):
    STDOUT.write('%04X\t%d\n' % (cp, count[cp]))
  return


if __name__ == '__main__':
  main(sys.argv)

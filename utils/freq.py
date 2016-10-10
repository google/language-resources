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

"""Sorts tokens by decreasing frequency.

Reads tokens from stdin (one per line). Sorts them by decreasing frequency as
primary key, with lexicographic ordering of strings as secondary key.
Writes sorted tokens to stdout (one per line).

"""

from __future__ import unicode_literals

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def main(unused_argv):
  token_count = {}
  for line in STDIN:
    token = line.rstrip('\n')
    token_count[token] = token_count.get(token, 0) + 1
  items = token_count.items()
  items.sort(key = lambda (token, count): (-count, token))
  for token, count in items:
    STDOUT.write('%s\t%d\n' % (token, count))
  return


if __name__ == '__main__':
  main(sys.argv)

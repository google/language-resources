#! /usr/bin/env python
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

"""Tokenize isiXhosa words according to word-internal capitalization.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDOUT = codecs.getwriter('utf-8')(sys.stdout)

TOKEN_RE = re.compile(r"([A-Z]|[A-Z]?['a-zćé])['a-zćé]*|[0-9]+")


def GetlineUnbuffered():
  while True:
    line = sys.stdin.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def Tokens(line):
  for token in line.split():
    for match in TOKEN_RE.finditer(token):
      yield match.group(0)
  return


if __name__ == '__main__':
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    STDOUT.write('%s\n%s\n\n' %
                 (line, ' '.join(Tokens(line))))

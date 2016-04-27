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

"""Converts a prompts file in TSV format to Festival's Scheme format.
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
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    fields = line.split('\t')
    assert len(fields) >= 2
    utterance_id = fields[0]
    prompt_text = fields[1]
    assert '"' not in prompt_text
    STDOUT.write('( %s "%s" )\n' % (utterance_id, prompt_text))
  return


if __name__ == '__main__':
  main(sys.argv)

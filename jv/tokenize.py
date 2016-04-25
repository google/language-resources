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

"""Simple tokenizer for Javanese sentences.
"""

from __future__ import unicode_literals

import codecs
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

SPLIT = re.compile(r'[/\s]+')
TOKEN = re.compile(r"[-'a-zâåèéêìòù]+$")


def main(unused_argv):
  for line in STDIN:
    for token in SPLIT.split(line.strip().rstrip('?').lower()):
      if TOKEN.match(token):
        writer = STDOUT
      else:
        writer = STDERR
      writer.write('%s\n' % token)
  return


if __name__ == '__main__':
  main(sys.argv)

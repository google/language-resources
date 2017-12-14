# -*- coding: utf-8 -*-
#
# Copyright 2016, 2017 Google Inc. All Rights Reserved.
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

import re

from utils import utf8

SPLIT = re.compile(r'[/\s]+')
TOKEN = re.compile(r"[-'a-zâåèéêìòù]+$")


def main(unused_argv):
  for line in utf8.stdin:
    for token in SPLIT.split(line.strip().rstrip('?').lower()):
      if TOKEN.match(token):
        writer = utf8.stdout
      else:
        writer = utf8.stderr
      writer.write('%s\n' % token)
  return


if __name__ == '__main__':
  main(None)

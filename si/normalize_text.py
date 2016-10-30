#! /usr/bin/python
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

"""Utility for normalizing Sinhala text.
"""

from __future__ import unicode_literals

import codecs
import sys
import unicodedata

import zero_width

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)


def NormalizeText(text):
  text = unicodedata.normalize('NFC', text)
  text = zero_width.RemoveOptionalZW(text)
  text = text.replace('\u0D85\u0DCF', '\u0D86')  # අා -> ආ
  text = text.replace('\u0D85\u0DD0', '\u0D87')  # අැ -> ඇ
  text = text.replace('\u0D85\u0DD1', '\u0D88')  # අෑ -> ඈ
  text = text.replace('\u0D8B\u0DDF', '\u0D8C')  # උෟ -> ඌ
  text = text.replace('\u0D91\u0DD9', '\u0D93')  # එෙ -> ඓ
  text = text.replace('\u0D91\u0DCA', '\u0D92')  # එ් -> ඒ
  text = text.replace('\u0D92\u0DCA', '\u0D92')  # ඒ් -> ඒ  (redundant virama)
  text = text.replace('\u0D94\u0DDF', '\u0D96')  # ඔෟ -> ඖ
  return text


if __name__ == '__main__':
  for line in STDIN:
    line = NormalizeText(line)
    STDOUT.write(line)

#! /usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright 2015 Google Inc. All Rights Reserved.
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

from __future__ import unicode_literals

import codecs
import importlib
import os.path
import re
import sys

from tools import grapheme_util
from tools import icu_util

STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)
STDERR = codecs.lookup('utf-8').streamwriter(sys.stderr)

if len(sys.argv) <= 1:
  STDERR.write('Usage: %s LANGUAGE\n' % sys.argv[0])
  sys.exit(1)

lang = sys.argv[1]
graphemes = importlib.import_module(lang + '.graphemes')

transform = '%s-%s_FONIPA' % (lang, lang)
rule_path = os.path.join(os.path.dirname(sys.argv[0]),
                         '%s/%s.txt' % (lang, transform))

xltor = icu_util.LoadTransliterationRules(rule_path, transform)

for line in grapheme_util.GetlineUnbuffered():
  for token in re.findall(graphemes.TOKEN_RE, line):
    symbolic = grapheme_util.StringToSymbols(token,
                                             graphemes.CODEPOINT_TO_SYMBOL)
    pron = xltor.transliterate(token)
    STDOUT.write('%s\t%s\t%s\n' % (token, symbolic, pron))
    STDOUT.flush()

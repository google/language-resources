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

import importlib
import sys

from utils import grapheme_util

if len(sys.argv) <= 1:
  sys.stderr.write('Usage: %s LANGUAGE\n' % sys.argv[0])
  sys.exit(1)

lang = sys.argv[1]
graphemes = importlib.import_module(lang + '.graphemes')
grapheme_util.TransliterateInteractively(graphemes.CODEPOINT_TO_SYMBOL)

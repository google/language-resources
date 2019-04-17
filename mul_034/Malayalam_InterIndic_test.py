# -*- coding: utf-8 -*-

# Copyright 2019 Google LLC. All Rights Reserved.
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

"""Test ICU transliteration to and from Malayalam via InterIndic."""

from __future__ import unicode_literals

import io
import os.path
import sys
import unittest

import icu  # requires patched version of PyICU that fixes registerInstance()


INSTANCES = {}


def Transliterate(name, text):
  if name not in INSTANCES:
    INSTANCES[name] = icu.Transliterator.createInstance(name)
  return INSTANCES[name].transliterate(text)


def ReadRules(path):
  with io.open(path, 'rt', encoding='utf8') as reader:
    in_body = False
    lines = []
    for line in reader:
      if '</tRule>' in line:
        break
      elif '<tRule>' in line:
        in_body = True
      elif in_body:
        lines.append(line)
    return ''.join(lines)


def RegisterTransliterator(path, name):
  rules = ReadRules(path)
  transliterator = icu.Transliterator.createFromRules(name, rules)
  icu.Transliterator.registerInstance(transliterator)
  return


class MalayalamInterIndicTest(unittest.TestCase):

  def testMalayalamBengali(self):
    for mlym, beng in [
        ('കോ', 'কো'),
        ('കൌ', 'কৌ'),
    ]:
      self.assertEqual(Transliterate('Mlym-Beng', mlym), beng)
      self.assertEqual(Transliterate('Beng-Mlym', beng), mlym)
    for mlym, beng in [
        ('കൊ', 'কো'),
        ('കൗ', 'কৌ'),
    ]:
      self.assertEqual(Transliterate('Mlym-Beng', mlym), beng)
    return

  def testMalayalamLatin(self):
    for mlym, latn in [
        ('കൊ', 'ko'),
        ('കോ', 'kō'),
        ('കൌ', 'kau'),
    ]:
      self.assertEqual(Transliterate('Mlym-Latn', mlym), latn)
      self.assertEqual(Transliterate('Latn-Mlym', latn), mlym)
    for mlym, latn in [
        ('കൗ', 'kau'),
        ('ൟ', 'ī'),
        ('൰', '10'),
        ('൱', '100'),
        ('൲', '1000'),
        ('ലയോൺ', 'layōṇ'),
        ('കുഞ്ചൻ', 'kuñcan'),
        ('തിരൂർ', 'tirūr'),
        ('എന്നാൽ', 'ennāl'),
        ('ഗൾഫ്', 'gaḷph'),
        ('ൿ', 'k'),
    ]:
      self.assertEqual(Transliterate('Mlym-Latn', mlym), latn)
    return

  def testMalayalamTamil(self):
    for mlym, taml in [
        ('ഔ', 'ஔ'),
        ('കൊ', 'கொ'),
        ('കോ', 'கோ'),
        ('കൌ', 'கௌ'),
    ]:
      self.assertEqual(Transliterate('Mlym-Taml', mlym), taml)
      self.assertEqual(Transliterate('Taml-Mlym', taml), mlym)
    for mlym, taml in [
        ('കൗ', 'கௌ'),
        ('തിരൂർ', 'திரூர்'),
    ]:
      self.assertEqual(Transliterate('Mlym-Taml', mlym), taml)
    return


def main(argv):
  directory = os.path.join(os.path.dirname(os.path.dirname(argv[0])),
                           'third_party', 'cldr')
  for filename, name in [
      ('Malayalam-InterIndic.xml', 'Malayalam-InterIndic'),
      ('Malayalam-Latin.xml', 'Mlym-Latn'),
      ('Malayalam-Bengali.xml', 'Mlym-Beng'),
      ('Malayalam-Tamil.xml', 'Mlym-Taml'),
  ]:
    RegisterTransliterator(os.path.join(directory, filename), name)
  unittest.main()
  return


if __name__ == '__main__':
  main(sys.argv)

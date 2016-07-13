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

"""Conversion from Zawgyi to Unicode.

Example usage:

cat ../third_party/voanews/burmese_*.txt |
./grapheme_cluster.py zawgyi |
fgrep Cluster |
cut -f2 |
../utils/freq.py |
./cluster_to_unicode.py |
./make_comparison_table.py > zawgyi_unicode.tex &&
lualatex zawgyi_unicode

"""

from __future__ import unicode_literals

import codecs
import sys
import unicodedata

STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)
STDERR = codecs.lookup('utf-8').streamwriter(sys.stderr)


def MergeStrings(first, second, kind):
  if first and second:
    if first != second:
      raise Exception('Conflicting %s: %s != %s\n' % (kind, first, second))
      return None
    else:
      STDERR.write('Redundant %s: %s\n' % (kind, first))
      return first
  if first:
    assert not second
    return first
  return second


class Cluster(object):

  def __init__(self, main='', kinzi='', below='',
               y='', r='', w='', h='',
               e='', i='', u='', a='',
               anusvara='', visarga='', dot='', asat=''):
    self.main = main
    self.kinzi = kinzi
    self.below = below
    self.asat1 = ''
    self.y = y
    self.r = r
    self.w = w
    self.h = h
    self.e = e
    self.i = i
    self.u = u
    self.a = a
    self.anusvara = anusvara
    self.visarga = visarga
    self.dot = dot
    self.asat = asat
    return

  def Merge(self, other):
    self.main = MergeStrings(self.main, other.main, 'main letter')
    self.kinzi = MergeStrings(self.kinzi, other.kinzi, 'kinzi')
    self.below = MergeStrings(self.below, other.below, 'subscripted consonant')
    self.y = MergeStrings(self.y, other.y, 'medial y')
    self.r = MergeStrings(self.r, other.r, 'medial r')
    self.w = MergeStrings(self.w, other.w, 'medial w')
    self.h = MergeStrings(self.h, other.h, 'medial h')
    self.e = MergeStrings(self.e, other.e, 'modifier e left')
    self.i = MergeStrings(self.i, other.i, 'modifier i above')
    self.u = MergeStrings(self.u, other.u, 'modifier u below')
    self.a = MergeStrings(self.a, other.a, 'modifier a right')
    self.anusvara = MergeStrings(self.anusvara, other.anusvara,
                                 'modifier anusvara above')
    self.visarga = MergeStrings(self.visarga, other.visarga,
                                 'modifier visarga right')
    self.dot = MergeStrings(self.dot, other.dot,
                                 'modifier dot below')
    if other.asat:
      if not self.a:
        self.asat1 = MergeStrings(self.asat1, other.asat, 'asat')
      else:
        self.asat = MergeStrings(self.asat, other.asat, 'asat')
    return

  def Debug(self, writer):
    writer.write('cluster {\n')
    if self.main:
      writer.write('      main: %s\n' % self.main)
    if self.kinzi:
      writer.write('     kinzi: %s\n' % self.kinzi)
    if self.below:
      writer.write('     below: %s\n' % self.below)
    if self.asat1:
      writer.write('     asat1: %s\n' % self.asat1)
    if self.y:
      writer.write('        -y: %s\n' % self.y)
    if self.r:
      writer.write('        -r: %s\n' % self.r)
    if self.w:
      writer.write('        -w: %s\n' % self.w)
    if self.h:
      writer.write('        -h: %s\n' % self.h)
    if self.e:
      writer.write('        -e: %s\n' % self.e)
    if self.i:
      writer.write('        -i: %s\n' % self.i)
    if self.u:
      writer.write('        -u: %s\n' % self.u)
    if self.a:
      writer.write('       -aa: %s\n' % self.a)
    if self.asat:
      writer.write('      asat: %s\n' % self.asat)
    if self.anusvara:
      writer.write('  anusvara: %s\n' % self.anusvara)
    if self.visarga:
      writer.write('   visarga: %s\n' % self.visarga)
    if self.dot:
      writer.write('       dot: %s\n' % self.dot)
    writer.write('}\n')
    return

  def ToString(self):
    if self.asat1 and self.i in ('\u102D', '\u102E'):
      # In Zawgyi, if a letter contains a superscript -i (102D) or -ii
      # (102E), then adding Asat (1039) will not be visible. It's therefore
      # plausible to assume that the presence of Asat was unintentional.
      STDERR.write('Dropping Asat in:\n')
      self.Debug(STDERR)
      self.asat1 = ''
    if self.main == '\u1025':
      if self.i == '\u102E':
        STDERR.write("Changing 'u -ii to 'uu in:\n")
        assert not self.asat1
        self.Debug(STDERR)
        self.main = '\u1026'
        self.i = ''
      if self.asat1 or self.a:
        STDERR.write("Changing 'u to nya in:\n")
        self.Debug(STDERR)
        self.main = '\u1009'
    if self.main == '\u101E' and self.r:
      if not self.e and not self.a:
        STDERR.write("Changing sa -r to 'o in:\n")
        self.Debug(STDERR)
        self.main = '\u1029'
        self.r = ''
      elif self.e and self.a and self.asat:
        STDERR.write("Changing sa -r -e -aa asat to 'au in:\n")
        self.Debug(STDERR)
        self.main = '\u102A'
        self.r = ''
        self.e = ''
        self.a = ''
        self.asat = ''
    if self.main == '\u1040' and (
        self.kinzi or self.below or self.asat1 or self.y or self.r or
        self.w or self.h or self.e or self.i or self.u or self.a or
        self.anusvara or self.dot or self.asat or self.visarga):
      STDERR.write('Changing zero to wa in:\n')
      self.Debug(STDERR)
      self.main = '\u101D'
    # What about sequence of 1032 1036? Dual top vowel
    # What about 1041 102C --> looks like 1018
    # "-u anusvara" might be a typo for "-u -i"
    # Z"100F 106D" might be a typo for U"100F 1039 100D" (not 100C)
    # Multiple 102C?
    s = ''
    if self.kinzi:
      s += self.kinzi
    if self.main:
      s += self.main
    if self.below:
      s += self.below
    if self.asat1:
      s += self.asat1
    if self.y:
      s += self.y
    if self.r:
      s += self.r
    if self.w:
      s += self.w
    if self.h:
      s += self.h
    if self.e:
      s += self.e
    if self.i:
      s += self.i
    if self.u:
      s += self.u
    if self.a:
      s += self.a
    if self.anusvara:
      s += self.anusvara
    if self.dot:
      s += self.dot
    if self.asat:
      s += self.asat
    if self.visarga:
      s += self.visarga
    return s


ZAWGYI_CLUSTERS = {
    0x0028: Cluster(main='('),
    0x0029: Cluster(main=')'),
    0x002D: Cluster(main='-'),
    0x1000: Cluster(main='\u1000'),
    0x1001: Cluster(main='\u1001'),
    0x1002: Cluster(main='\u1002'),
    0x1003: Cluster(main='\u1003'),
    0x1004: Cluster(main='\u1004'),
    0x1005: Cluster(main='\u1005'),
    0x1006: Cluster(main='\u1006'),
    0x1007: Cluster(main='\u1007'),
    0x1008: Cluster(main='\u1008'),
    0x1009: Cluster(main='\u1009'),
    0x100A: Cluster(main='\u100A'),
    0x100B: Cluster(main='\u100B'),
    0x100C: Cluster(main='\u100C'),
    0x100D: Cluster(main='\u100D'),
    0x100E: Cluster(main='\u100E'),
    0x100F: Cluster(main='\u100F'),
    0x1010: Cluster(main='\u1010'),
    0x1011: Cluster(main='\u1011'),
    0x1012: Cluster(main='\u1012'),
    0x1013: Cluster(main='\u1013'),
    0x1014: Cluster(main='\u1014'),
    0x1015: Cluster(main='\u1015'),
    0x1016: Cluster(main='\u1016'),
    0x1017: Cluster(main='\u1017'),
    0x1018: Cluster(main='\u1018'),
    0x1019: Cluster(main='\u1019'),
    0x101A: Cluster(main='\u101A'),
    0x101B: Cluster(main='\u101B'),
    0x101C: Cluster(main='\u101C'),
    0x101D: Cluster(main='\u101D'),
    0x101E: Cluster(main='\u101E'),
    0x101F: Cluster(main='\u101F'),
    0x1020: Cluster(main='\u1020'),
    0x1021: Cluster(main='\u1021'),
    # 1022 is undefined
    0x1023: Cluster(main='\u1023'),
    0x1024: Cluster(main='\u1024'),
    0x1025: Cluster(main='\u1025'),
    0x1026: Cluster(main='\u1026'),
    0x1027: Cluster(main='\u1027'),
    # 1028 is undefined
    0x102A: Cluster(main='\u102A'),
    0x102B: Cluster(a='\u102B'),
    0x102C: Cluster(a='\u102C'),
    0x102D: Cluster(i='\u102D'),  # redundant occurrences can be ignored
    0x102E: Cluster(i='\u102E'),
    0x102F: Cluster(u='\u102F'),
    0x1030: Cluster(u='\u1030'),
    0x1031: Cluster(e='\u1031'),
    0x1032: Cluster(i='\u1032'),
    0x1033: Cluster(u='\u102F'),
    0x1034: Cluster(u='\u1030'),
    # 1035 is undefined
    0x1036: Cluster(anusvara='\u1036'),
    0x1037: Cluster(dot='\u1037'),
    0x1038: Cluster(visarga='\u1038'),
    0x1039: Cluster(asat='\u103A'),
    0x103A: Cluster(y='\u103B'),
    0x103B: Cluster(r='\u103C'),
    0x103C: Cluster(w='\u103D'),
    0x103D: Cluster(h='\u103E'),
    # 103E is undefined
    # 103F is undefined
    0x1040: Cluster(main='\u1040'),
    0x1041: Cluster(main='\u1041'),
    0x1042: Cluster(main='\u1042'),
    0x1043: Cluster(main='\u1043'),
    0x1044: Cluster(main='\u1044'),
    0x1045: Cluster(main='\u1045'),
    0x1046: Cluster(main='\u1046'),
    0x1047: Cluster(main='\u1047'),
    0x1048: Cluster(main='\u1048'),
    0x1049: Cluster(main='\u1049'),
    0x104A: Cluster(main='\u104A'),
    0x104B: Cluster(main='\u104B'),
    0x104D: Cluster(main='\u104D'),
    # TODO: Need to treat 104E as an exception, as it consists of 2 clusters.
    0x104F: Cluster(main='\u104F'),
    # 1050 through 105F are undefined except for 105A
    0x105A: Cluster(a='\u102B\u103A'),
    0x1060: Cluster(below='\u1039\u1000'),
    0x1061: Cluster(below='\u1039\u1001'),
    0x1062: Cluster(below='\u1039\u1002'),
    0x1063: Cluster(below='\u1039\u1003'),
    0x1064: Cluster(kinzi='\u1004\u103A\u1039'),
    0x1065: Cluster(below='\u1039\u1005'),
    0x1066: Cluster(below='\u1039\u1006'),
    0x1067: Cluster(below='\u1039\u1006'),
    0x1068: Cluster(below='\u1039\u1007'),
    0x1069: Cluster(below='\u1039\u1008'),
    0x106A: Cluster(main='\u1009'),
    0x106B: Cluster(main='\u100A'),
    0x106C: Cluster(below='\u1039\u100B'),
    0x106D: Cluster(below='\u1039\u100C'),
    0x106E: Cluster(main='\u100D', below='\u1039\u100D'),
    0x106F: Cluster(main='\u100E', below='\u1039\u100D'),
    0x1070: Cluster(below='\u1039\u100F'),
    0x1071: Cluster(below='\u1039\u1010'),
    0x1072: Cluster(below='\u1039\u1010'),
    0x1073: Cluster(below='\u1039\u1011'),
    0x1074: Cluster(below='\u1039\u1011'),
    0x1075: Cluster(below='\u1039\u1012'),
    0x1076: Cluster(below='\u1039\u1013'),
    0x1077: Cluster(below='\u1039\u1014'),
    0x1078: Cluster(below='\u1039\u1015'),
    0x1079: Cluster(below='\u1039\u1016'),
    0x107A: Cluster(below='\u1039\u1017'),
    0x107B: Cluster(below='\u1039\u1018'),
    0x107C: Cluster(below='\u1039\u1019'),
    0x107D: Cluster(y='\u103B'),
    0x107E: Cluster(r='\u103C'),
    0x107F: Cluster(r='\u103C'),
    0x1080: Cluster(r='\u103C'),
    0x1081: Cluster(r='\u103C'),
    0x1082: Cluster(r='\u103C'),
    0x1083: Cluster(r='\u103C'),
    0x1084: Cluster(r='\u103C'),
    0x1085: Cluster(below='\u1039\u101C'),
    0x1086: Cluster(main='\u103F'),
    0x1087: Cluster(h='\u103E'),
    0x1088: Cluster(h='\u103E', u='\u102F'),
    0x1089: Cluster(h='\u103E', u='\u1030'),
    0x108A: Cluster(w='\u103D', h='\u103E'),
    0x108B: Cluster(kinzi='\u1004\u103A\u1039', i='\u102D'),
    0x108C: Cluster(kinzi='\u1004\u103A\u1039', i='\u102E'),
    0x108D: Cluster(kinzi='\u1004\u103A\u1039', anusvara='\u1036'),
    0x108E: Cluster(i='\u102D', anusvara='\u1036'),
    0x108F: Cluster(main='\u1014'),
    0x1090: Cluster(main='\u101B'),
    0x1091: Cluster(main='\u100F', below='\u1039\u100D'),
    0x1092: Cluster(main='\u100B', below='\u1039\u100C'),
    0x1093: Cluster(below='\u1039\u1018'),
    0x1094: Cluster(dot='\u1037'),
    0x1095: Cluster(dot='\u1037'),
    0x1096: Cluster(below='\u1039\u1010', w='\u103D'),
    0x1097: Cluster(main='\u100B', below='\u1039\u100B'),
}


def GetlineUnbuffered(f=sys.stdin):
  while True:
    line = f.readline()
    if not line:
      break
    yield line.decode('utf-8')
  return


def main():
  for line in GetlineUnbuffered():
    line = line.rstrip('\n')
    fields = line.split('\t')
    text = fields[0]
    if len(text) > 10:
      STDERR.write('Skipping long line: %s\n' % line)
      continue
    try:
      cluster = Cluster()
      for c in text:
        cluster.Merge(ZAWGYI_CLUSTERS[ord(c)])
      u = cluster.ToString()
    except Exception, e:
      STDERR.write('Exception: %s\n' % e)
      continue
    w = unicodedata.normalize('NFC', u)
    if u != w:
      STDERR.write('%s normalized to %s\n' % (repr(u), repr(w)))
    STDOUT.write('%s\t%s\n' % (text, w))


if __name__ == '__main__':
  main()

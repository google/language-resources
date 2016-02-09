#! /usr/bin/env python

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

"""Check whether a given variable-length code is uniquely decodable.

This is a direct/naive implementation of the Sardinas-Patterson algorithm.
It can be used to check if e.g. a given phoneme inventory yields unambiguous
transcriptions.
"""


def LeftQuotientOfWord(ps, w):
  """Yields the suffixes of w after removing any prefix in ps."""
  for p in ps:
    if w.startswith(p):
      yield w[len(p):]
  return


def LeftQuotient(ps, ws):
  """Returns the set of suffixes of any word in ws after removing any prefix
  in ps. This is the quotient set which results from dividing ws on the
  left by ps."""
  qs = set()
  for w in ws:
    for q in LeftQuotientOfWord(ps, w):
      qs.add(q)
  return qs


def IsUniquelyDecodable(cs, err):
  """Checks if the set of codewords cs is uniquely decodable via the
  Sardinas-Patterson algorithm. Prints diagnostic output to err."""
  s = LeftQuotient(cs, cs)
  s.discard('')
  if len(s) == 0:
    err.write('Uniquely decodable prefix code.\n')
    return True
  while '' not in s and len(s & cs) == 0:
    t = LeftQuotient(cs, s) | LeftQuotient(s, cs)
    if t == s:
      err.write('Uniquely decodable.\n')
      return True
    s = t
  if '' in s:
    err.write('Dangling empty suffix.\n')
  for x in s & cs:
    err.write('Dangling suffix: %s\n' % x)
  return False


if __name__ == '__main__':
  import sys
  import codecs

  cs = set()
  if len(sys.argv) > 1:
    with codecs.open(sys.argv[1], 'r', 'utf-8') as reader:
      for line in reader:
        for c in line.split():
          cs.add(c)
  else:
    for line in codecs.lookup('utf-8').streamreader(sys.stdin):
      for c in line.split():
        cs.add(c)

  err = codecs.lookup('utf-8').streamwriter(sys.stdout)
  uniquely_decodable = IsUniquelyDecodable(cs, err)
  sys.exit(0 if uniquely_decodable else 1)

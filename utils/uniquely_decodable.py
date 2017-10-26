# Copyright 2015, 2017 Google Inc. All Rights Reserved.
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

from __future__ import unicode_literals

import sys

import utf8


def LeftQuotientOfWord(ps, w):
  """Yields the suffixes of w after removing any prefix in ps."""
  for p in ps:
    if w.startswith(p):
      yield w[len(p):]
  return


def LeftQuotient(ps, ws):
  r"""Returns the quotient set ps \ ws.

  Returns the set of suffixes of any word in ws after removing any prefix
  in ps. This is the quotient set which results from dividing ws on the
  left by ps.

  Args:
      ps: set of prefixes to be remvoed from words
      ws: set of words

  Returns:
     quotient set ps \ ws
  """
  qs = set()
  for w in ws:
    for q in LeftQuotientOfWord(ps, w):
      qs.add(q)
  return qs


def IsUniquelyDecodable(cs, err):
  """Checks if a set of codewords is uniquely decodable.

  Checks if the set of codewords cs is uniquely decodable via the
  Sardinas-Patterson algorithm. Prints diagnostic output to err.

  Args:
      cs: set of codewords
      err: output stream for capturing errors

  Returns:
      True iff the set of codewords is uniquely decodable
  """
  s = LeftQuotient(cs, cs)
  s.discard('')
  if not s:
    err.write('Uniquely decodable prefix code.\n')
    return True
  while '' not in s and not s & cs:
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


def main(argv):
  cs = set()
  if len(argv) > 1:
    with utf8.open(argv[1]) as reader:
      for line in reader:
        for c in line.split():
          cs.add(c)
  else:
    for line in utf8.stdin:
      for c in line.split():
        cs.add(c)

  uniquely_decodable = IsUniquelyDecodable(cs, utf8.stderr)
  sys.exit(0 if uniquely_decodable else 1)
  return


if __name__ == '__main__':
  main(sys.argv)

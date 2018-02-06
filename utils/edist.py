#! /usr/bin/env python

# Copyright 2008, 2018 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


"""Weighted string edit distance.
"""

__author__ = 'mjansche@google.com (Martin Jansche)'


_INF = 1e300 * 1e300


class EditDistance(object):
  """Computes the weighted memoryless edit distance between two sequences."""

  START = 'S'
  DELETION = 'D'
  INSERTION = 'I'
  MATCH = 'M'

  def __init__(self, x, y, cost):
    """Compute a weighted edit distance.

    Compute the weighted edist distance between 'x' and 'y'
    w.r.t. the cost function 'cost'.  The cost function must return
    values that can be added and compared against each other and
    against integers.  It will be invoked as 'cost(None, y_j)' to
    return the insertion cost of 'y_j'; as 'cost(x_i, None)' to return
    the deletion cost of 'x_i'; and as 'cost(x_i, y_j)' to return the
    cost of matching 'x_i' with 'y_j'.

    Args:
        x: first sequence
        y: second sequence
        cost: function returning edit costs
    """
    x = tuple(x)
    y = tuple(y)

    # This is the standard Levenshtein-Viterbi-Vintsyuk-Needleman-Wunsch-
    # Velichko-Zagoruyko-Sakoe-Chiba-Sankoff-Reichert-Cohen-Wong-Haton-
    # Wagner-Fischer-Hirschberg dynamic programming algorithm.

    m = len(x) + 1
    n = len(y) + 1
    M = {(0, 0): (self.START, 0)}

    for j in range(1, n):
      M[0, j] = (self.INSERTION, M[0, j-1][1] + cost(None, y[j-1]))

    for i in range(1, m):
      M[i, 0] = (self.DELETION, M[i-1, 0][1] + cost(x[i-1], None))
      for j in range(1, n):
        # Start with insertion:
        a = self.INSERTION
        v = M[i, j-1][1] + cost(None, y[j-1])
        if True:
          argmin = a
          valmin = v
        # Consider deletion:
        a = self.DELETION
        v = M[i-1, j][1] + cost(x[i-1], None)
        if v < valmin:
          argmin = a
          valmin = v
        # Consider match/substitution:
        a = self.MATCH
        v = M[i-1, j-1][1] + cost(x[i-1], y[j-1])
        if v < valmin:
          argmin = a
          valmin = v
        # Update with local minimum:
        M[i, j] = (argmin, valmin)

    self.x = x
    self.y = y
    self.m = m
    self.n = n
    self.M = M
    return

  def PrettyPrint(self, writer):
    """Prints the dynamic programming matrix (for debugging)."""
    for i in range(self.m):
      for j in range(self.n):
        if (i, j) in self.M:
          writer.write('  %s: %s' % self.M[i, j])
      writer.write('\n')
    return

  def Valmin(self):
    """Returns the cost of the least-cost alignment."""
    return self.M[self.m-1, self.n-1][1]

  def Argmin(self, eps1=None, eps2=None):
    """Returns the least-cost alignment."""
    alignment = []
    i = self.m - 1
    j = self.n - 1
    while True:
      a, _ = self.M[i, j]
      if a == self.START:
        assert (0, 0) == (i, j)
        break
      elif a == self.DELETION:
        i -= 1
        alignment.append((self.x[i], eps2))
      elif a == self.INSERTION:
        j -= 1
        alignment.append((eps1, self.y[j]))
      else:
        assert a == self.MATCH
        i -= 1
        j -= 1
        alignment.append((self.x[i], self.y[j]))
    alignment.reverse()
    return alignment


def LevenshteinCost(a, b):
  """Cost function for Levenshtein distance with substitutions.

  Cost function for what is now thought of as the classical
  Levenshtein distance, which is the minimum number of insertions,
  deletions, and substitutions required to edit one sequence into
  another.  Returns zero for matches, unity for indels and
  substitutions.

  Args:
      a: input symbol, or None for insertions
      b: output symbol, or None for deletions

  Returns:
      0 for exact match
      1 for mismatch / substitution
      1 for insertion or deletion
  """
  if a == b:
    return 0
  else:
    return 1


def LevenshteinCostNoSubs(a, b):
  """Cost function for Levenshtein distance without substitutions.

  Cost function for Levenshtein's other edit distance (the orginal,
  no substitutions), which is the minimum number of indels required to
  edit one sequence into another.  Returns zero for matches, unity for
  indels, and infinity for substitutions.

  Args:
      a: input symbol, or None for insertions
      b: output symbol, or None for deletions

  Returns:
      0   for match
      inf for mismatch / substitution
      1   for insertion or deletion
  """
  if a == b:
    return 0
  elif a is None or b is None:
    return 1
  else:
    return _INF


def HammingCost(a, b):
  """Cost function for Hamming distance.

  Returns zero for matches, one for mismatches, and infinity for
  indels.  The overall edit distance is finite iff the two sequences
  have the same length.  This function is really only useful for
  debugging, since the naive algorithm for computing the Hamming
  distance is more efficient than the dynamic programming algorithm.

  Args:
      a: input symbol, or None for insertions
      b: output symbol, or None for deletions

  Returns:
      0   for match
      1   for mismatch / substitution
      inf for insertion or deletion
  """
  if a == b:
    return 0
  elif a is None or b is None:
    return _INF
  else:
    return 1


if __name__ == '__main__':
  import sys
  x, y = sys.argv[1:3]
  print(x, y)
  ed = EditDistance(x, y, LevenshteinCost)
  ed.PrettyPrint(sys.stdout)
  print(ed.Argmin())
  print(ed.Valmin())

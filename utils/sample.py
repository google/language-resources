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

"""Random sampling of N lines from stdin.

Reads lines from stdin and writes N randomly sampled lines to stdout,
all other lines to stderr.
"""

import random
import sys

from utils import utf8


def Sample(iterable, n, rng):
  """Samples n items from a stream.

  Args:
    iterable: An iterable stream of items.
    n: The target sample size.
    rng: A random number generator that supports rng.randint(a, b).

  Yields:
    (True,  item) for in-sample items;
    (False, item) for out-of-sample items.
  """
  reservoir = []
  for i, item in enumerate(iterable):
    if i < n:
      reservoir.append(item)
      continue
    r = rng.randint(0, i)
    if r < n:
      yield False, reservoir[r]
      reservoir[r] = item
    else:
      yield False, item
  for item in reservoir:
    yield True, item
  return


def main(argv):
  if len(argv) != 2:
    utf8.Print('Usage: %s SAMPLE_SIZE' % argv[0])
    sys.exit(2)
  n = int(argv[1])
  for in_sample, line in Sample(utf8.stdin, n, random.Random()):
    if in_sample:
      utf8.stdout.write(line)
    else:
      utf8.stderr.write(line)
  return


if __name__ == '__main__':
  main(sys.argv)

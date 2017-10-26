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

"""Simple evaluation of a prounciation dictionary.
"""

from __future__ import unicode_literals

import sys

import utf8

STDIN = utf8.stdin
STDOUT = utf8.stdout
STDERR = utf8.stderr


def ReadTSV(reader):
  for line in reader:
    line = line.rstrip('\n')
    if not(line) or line.startswith('#'):
      continue
    fields = line.split('\t')
    if len(fields) < 2:
      continue
    yield fields[:2]
  return


def main(args):
  if len(args) < 2 or len(args) > 3:
    STDOUT.write('Usage: %s GOLDEN_DICTIONARY\n' % args[0])
    sys.exit(1)

  if len(args) == 3:
    target = float(args[2])
  else:
    target = -1

  golden = {}
  with utf8.open(args[1]) as reader:
    for key, val in ReadTSV(reader):
      if key not in golden:
        golden[key] = []
      golden[key].append(val)

  total = 0
  correct = 0
  for key, val in ReadTSV(STDIN):
    if key not in golden:
      STDOUT.write('Skipping %s; not in golden dictionary\n')
      continue
    total += 1
    if val in golden[key]:
      correct += 1

  if total == 0:
    STDOUT.write('Total number of words that can be evaluated is zero.\n')
    sys.exit(2)

  accuracy = correct * 100.0 / total
  STDOUT.write('Accuracy: %d / %d = %g %%\n' % (correct, total, accuracy))
  STDERR.write('Accuracy: %d / %d = %g %%\n' % (correct, total, accuracy))

  if target < 0:
    sys.exit(0)
  elif accuracy > target:
    STDERR.write('PASS\n')
    sys.exit(0)
  else:
    STDERR.write('FAIL\n')
    sys.exit(1)
  return


if __name__ == '__main__':
  main(sys.argv)

#! /usr/bin/env python
#
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

"""Convert logits of posterior labeling distribution to OpenFst text format."""

from __future__ import print_function

import io
import math
import os.path
import subprocess
import sys

import numpy as np


def LogitsToFstText(logits):
  """Convert matrix of logits into textual FST description."""
  frames, labels = logits.shape
  sink = io.StringIO()
  for f in range(frames):
    # Normalizing denominator of softmax:
    z = math.log(math.fsum(np.exp(logits[f], dtype=np.float64)))
    for l in range(labels):
      print(f, f + 1, l + 1, z - logits[f, l], file=sink)
  print(frames, file=sink)
  return sink.getvalue()


def WriteTxt(basename, contents):
  """Write contents to text file."""
  with io.open(basename + '.txt', 'wt') as writer:
    writer.write(contents)
  return


def WriteFst(basename, contents, fstcompile, fstsymbols, symbols):
  """Compile textual FST description and embed symbol tables."""
  fst_path = basename + '.fst'
  # Compile textual FST description from stdin into binary FST file:
  compile_argv = [
      fstcompile,
      '--acceptor',
      '--arc_type=log64',
      '-',
      fst_path,
  ]
  proc = subprocess.Popen(compile_argv, stdin=subprocess.PIPE)
  proc.communicate(input=contents.encode('ascii'))
  if proc.returncode != 0:
    sys.stderr.write('fstcompile failed: %s' % ' '.join(compile_argv))
  # Attach symbol tables to binary FST file:
  symbols_argv = [
      fstsymbols,
      '--isymbols=%s' % symbols,
      '--osymbols=%s' % symbols,
      '--save_isymbols',
      '--save_osymbols',
      fst_path,
      fst_path,
  ]
  subprocess.check_call(symbols_argv)
  return


def main(argv):
  for path in argv[1:]:
    logits = np.load(path)
    contents = LogitsToFstText(logits)
    # print(contents)
    basename = os.path.splitext(os.path.basename(path))[0]
    # WriteTxt(basename, contents)
    WriteFst(basename, contents,
             '../bazel-bin/external/openfst/fstcompile',
             '../bazel-bin/external/openfst/fstsymbols',
             'symbols.txt')
  return


if __name__ == '__main__':
  main(sys.argv)

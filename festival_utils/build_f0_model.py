#! /usr/bin/env python
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
"""Script is used to build F0 tree model.
"""

import io
import os
import sys

STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)
STDERR = io.open(2, mode='wt', encoding='utf-8', closefd=False)

# F0 wagon description file.
F0_DESCRIPTION_FILE = "festival/clunits/f0.desc"
# Path to wagon binary.
WAGON_BIN = "$ESTDIR/bin/wagon"

# Features to be ignored.
IGNORE = "-ignore '(R:mcep_link.parent.lisp_duration)'"
BALANCE = "-balance 0"
STOP = "-stop 200 "

# Festival features directory.
FESTIVAL_FEATS_PATH = "festival/feats/"
# Path to output trees.
OUTPUT_TREE_PATH = "festival/trees/"

# List of file extensions
FEATS_IDENTIFIED = "_f0"
FEATS_FILE_EXTENSION = ".feats"
OUTPUT_TREE_FILE_EXTENSION = ".tree"


def main(args):
  if len(args) == 1:
    STDOUT.write(
        "Usage: python scripts/build_f0_model.py festival/feats/*f0.feats")

  # TODO: Get states from statesname file.
  for i in xrange(1, len(args)):
    file_name = os.path.basename(args[i])
    state = file_name.replace(FEATS_IDENTIFIED + FEATS_FILE_EXTENSION, "")
    BuildWagonCommand(state)


def BuildWagonCommand(state):
  """Generates the wagon command to build the tree.

  Args :
    state: HMM state for which the F0 tree should be generated for.
  """
  cmd = "%s %s -desc %s " % (WAGON_BIN, IGNORE, F0_DESCRIPTION_FILE)
  cmd += "%s %s " % (BALANCE, STOP)
  cmd += "-data '%s%s%s%s' " % (FESTIVAL_FEATS_PATH, state, FEATS_IDENTIFIED,
                                FEATS_FILE_EXTENSION)
  cmd += "-test '%s%s%s%s' " % (FESTIVAL_FEATS_PATH, state, FEATS_IDENTIFIED,
                                FEATS_FILE_EXTENSION)
  cmd += "-output  '%s%s%s%s' " % (OUTPUT_TREE_PATH, state, FEATS_IDENTIFIED,
                                   OUTPUT_TREE_FILE_EXTENSION)

  STDOUT.write("[INFO] Generating tree for state " + state + "\n")
  STDOUT.write("[INFO] \n" + cmd + "\n")

  os.system(cmd)
  return


if __name__ == "__main__":
  main(sys.argv)

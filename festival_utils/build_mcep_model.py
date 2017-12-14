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
"""Script is used to build MCEP tree model.
"""

import io
import os
import sys

STDOUT = io.open(1, mode="wt", encoding="utf-8", closefd=False)
STDERR = io.open(2, mode="wt", encoding="utf-8", closefd=False)

# F0 wagon description file.
F0_DESCRIPTION_FILE = "festival/clunits/mcep.desc"
# Path to wagon binary.
WAGON_BIN = "$ESTDIR/bin/wagon"

BALANCE = "-balance 0"
STOP = "-stop 50"
VERTEX_OUTPUT = "mean"
TRACK_FEATS = "1-50"

# Festival features directory.
FESTIVAL_FEATS_PATH = "festival/feats/"
# Path to output trees.
OUTPUT_TREE_PATH = "festival/trees/"
DISTLABS_PATH = "festival/disttabs/"

# List of file extensions
MCEP_FILE_SUFFIX = "_mcep"
MCEP_FILE_EXTENSION = ".mcep"
FEATS_FILE_EXTENSION = ".feats"
OUTPUT_TREE_FILE_EXTENSION = ".tree"


def main(args):
  if len(args) == 1:
    STDOUT.write(
        "Usage: python scripts/build_mcep_model.py festival/disttabs/*.mcep")

  # TODO: Get states from statesname file.
  for i in xrange(1, len(args)):
    file_name = os.path.basename(args[i])
    state = file_name.replace(MCEP_FILE_EXTENSION, "")
    BuildWagonCommand(state)


def BuildWagonCommand(state):
  """Build wagon command to build MCEP model for the given state.

  Example command -
  $ESTDIR/bin/wagon
  -track_feats 1-50
  -vertex_output mean
  -desc festival/clunits/mcep.desc
  -data 'festival/feats/w_3.feats'
  -test 'festival/feats/w_3.feats'
  -balance 0
  -track 'festival/disttabs/w_3.mcep'
  -stop 50
  -output 'festival/trees/w_3_mcep.tree

  Args :
    state: HMM state for which the MCEP tree should be generated for.
  """

  cmd = "%s -desc %s " % (WAGON_BIN, F0_DESCRIPTION_FILE)
  cmd += "-track_feats %s -vertex_output %s %s %s " % (TRACK_FEATS,
                                                       VERTEX_OUTPUT, BALANCE,
                                                       STOP)
  cmd += "-data %s%s%s " % (FESTIVAL_FEATS_PATH, state, FEATS_FILE_EXTENSION)
  cmd += "-track %s%s%s " % (DISTLABS_PATH, state, MCEP_FILE_EXTENSION)
  cmd += "-test %s%s%s " % (FESTIVAL_FEATS_PATH, state, FEATS_FILE_EXTENSION)
  cmd += "-output %s%s%s%s " % (OUTPUT_TREE_PATH, state, MCEP_FILE_SUFFIX,
                                OUTPUT_TREE_FILE_EXTENSION)

  STDOUT.write("[INFO] Generating tree for state %s\n" % (state))
  STDOUT.write("[INFO] \n %s \n" % (cmd))

  os.system(cmd)
  return


if __name__ == "__main__":
  main(sys.argv)

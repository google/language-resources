# Copyright 2017 Google LLC. All Rights Reserved.
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

"""Writes OpenFst SymbolTables for various South (East) Asian scripts.
"""

from __future__ import unicode_literals

import os.path
import sys

from absl import app
from absl import flags
from mul_034 import script_util

FLAGS = flags.FLAGS

flags.DEFINE_bool('brahmic', False, 'Include all Brahmic scripts')
flags.DEFINE_bool('india', False, 'Include the 9 official scripts of India')
flags.DEFINE_bool('genrule', False, 'For use as a genrule tool')


def BasenameWithoutSuffix(path):
  basename = os.path.basename(path)
  return basename.rsplit('.', 1)[0]


def FilenamesForScripts(scripts):
  return [(s, '%s.syms' % s) for s in scripts]


def main(argv):
  scripts_paths = []
  if FLAGS.genrule:
    scripts_paths.extend(
        [(BasenameWithoutSuffix(path), path) for path in argv[1:]])
  else:
    if FLAGS.india or FLAGS.brahmic:
      scripts_paths.extend(
          FilenamesForScripts(script_util.BRAHMIC_OFFICIAL_INDIA))
    if FLAGS.brahmic:
      scripts_paths.extend(FilenamesForScripts(script_util.BRAHMIC_OTHER))
    scripts_paths.extend(FilenamesForScripts(argv[1:]))

  success = True
  for s, path in scripts_paths:
    script = script_util.GetScript(s)
    if not script:
      success = False
      continue
    symbols_and_labels = list(script_util.ScriptSymbols(script))
    if not script_util.IsBijectiveMapping(symbols_and_labels):
      success = False
      continue
    script_util.SymbolsToFile(path, symbols_and_labels)
  sys.exit(0 if success else 1)
  return


if __name__ == '__main__':
  app.run(main)

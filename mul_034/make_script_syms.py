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

import argparse
import os.path
import sys

from mul_034 import script_util


def BasenameWithoutSuffix(path):
  basename = os.path.basename(path)
  return basename.rsplit('.', 1)[0]


def FilenamesForScripts(scripts):
  return [(s, '%s.syms' % s) for s in scripts]


def main():
  parser = argparse.ArgumentParser(
      description='Writes OpenFST symbol tables for various scripts.')
  parser.add_argument('--brahmic', action='store_const', const=True,
                      help='Include all Brahmic scripts')
  parser.add_argument('--india', action='store_const', const=True,
                      help='Include the 9 official scripts of India')
  parser.add_argument('--genrule', action='store_const', const=True,
                      help='For use as a genrule tool')
  parser.add_argument('script', nargs='*', help='script name or code')
  args = parser.parse_args()

  scripts_paths = []
  if args.genrule:
    scripts_paths.extend(
        [(BasenameWithoutSuffix(path), path) for path in args.script])
  else:
    if args.india or args.brahmic:
      scripts_paths.extend(
          FilenamesForScripts(script_util.BRAHMIC_OFFICIAL_INDIA))
    if args.brahmic:
      scripts_paths.extend(FilenamesForScripts(script_util.BRAHMIC_OTHER))
    scripts_paths.extend(FilenamesForScripts(args.script))

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
  main()

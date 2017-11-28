#! /usr/bin/env python
#
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

"""Tool for tidying the format of WordList_IPA_SAMPA.csv.

Example usage:

$ ./clean_pron_dict_csv.py ../third_party/pron_dict/WordList_IPA_SAMPA.csv 

This tidies up the input CSV file and adds row numbers, to simplify
processing by subsequent tools.
"""

from __future__ import unicode_literals

import codecs
import sys

STDIN  = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)


def main():
  if len(sys.argv) > 1:
    reader = codecs.open(sys.argv[1], 'r', 'utf-8')
  else:
    reader = STDIN

  if len(sys.argv) > 2:
    writer = codecs.open(sys.argv[2], 'w', 'utf-8')
  else:
    writer = STDOUT

  row = 0
  for line in reader:
    row += 1
    if 'SAMPA' in line:
      continue
    line = line.rstrip('\r\n')
    fields = line.split(',')
    assert len(fields) == 4
    fields = [f.strip('"') for f in fields]
    for field in fields:
      assert '"' not in field
    if fields[0] == '':
      continue
    assert fields[3] in ('', '\\')
    writer.write('%s\n' % ','.join([str(row)] + fields[0:3]))

  reader.close()
  writer.close()
  return


if __name__ == '__main__':
  main()

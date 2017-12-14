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

"""Converts a pronunciation lexicon in TSV format to Festival lexicon format.
"""

from __future__ import unicode_literals

from utils import utf8


def main(unused_args):
  utf8.Print('MNCL')
  for line in utf8.stdin:
    line = line.rstrip('\n')
    # Skip comments
    if not line or line.startswith('#'):
      continue
    fields = line.split('\t')
    assert len(fields) >= 2
    orth = fields[0]
    pron = fields[1]
    sylls = ' '.join('((%s) 1)' % s for s in pron.split(' . '))
    utf8.Print('("%s" nil (%s))' % (orth, sylls))
  return


if __name__ == '__main__':
  main(None)

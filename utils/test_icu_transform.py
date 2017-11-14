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

"""Test general ICU transforms against a dictionary.
"""

from __future__ import unicode_literals

import sys

from utils import icu_util
from utils import utf8


def main(argv):
  if len(argv) != 3:
    utf8.Print('Usage: %s RULES DICTIONARY' % argv[0])
    sys.exit(2)

  xltor = icu_util.LoadTransliterationRules(argv[1], 'foo-bar')
  success = True
  with utf8.open(argv[2]) as reader:
    for line in reader:
      fields = line.rstrip('\n').split('\t')
      assert len(fields) >= 2
      orth, pron = fields[:2]
      predicted = xltor.transliterate(orth)
      if predicted != pron:
        utf8.Print('%s\t%s != %s' % (orth, pron, predicted))
        success = False

  if success:
    utf8.Print('PASS')
    sys.exit(0)
  else:
    utf8.Print('FAIL')
    sys.exit(1)
  return


if __name__ == '__main__':
  main(sys.argv)

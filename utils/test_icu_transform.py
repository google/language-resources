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

"""Test general ICU transforms against a list of (source, target) pairs.
"""

from __future__ import unicode_literals

import sys

from utils import icu_util
from utils import utf8


def main(argv):
  if len(argv) != 2:
    utf8.Print('Usage: test_icu_transform RULES')
    sys.exit(2)

  xltor = icu_util.LoadTransliterationRules(argv[1], 'foo-bar')
  success = True
  for line in utf8.stdin:
    fields = line.rstrip('\n').split('\t')
    assert len(fields) >= 2
    source, target = fields[:2]
    predicted = xltor.transliterate(source)
    if predicted != target:
      utf8.Print('%s\t%s != %s' % (source, target, predicted))
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

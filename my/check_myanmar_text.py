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

"""Checks that Myanmar text uses canonical diacritic storage order.
"""

from __future__ import unicode_literals

import re
import sys

from third_party.unicode import utn11
from utils import utf8

MYANMAR_TEXT = re.compile(r'[\u1000-\u109F\uAA60-\uAA7F\uA9E0-\uA9FF]+')


def main(unused_args):
  all_ok = True
  for line in utf8.stdin:
    line_ok = True
    for span in MYANMAR_TEXT.findall(line):
      if not utn11.CLUSTERS.match(span):
        utf8.stderr.write('"%s" is not in canonical storage order in line %s'
                          % (span, line))
        line_ok = False
    if line_ok:
      utf8.stdout.write(line)
    else:
      all_ok = False
  sys.exit(0 if all_ok else 1)
  return


if __name__ == '__main__':
  main(sys.argv)

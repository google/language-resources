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

"""Sorts tokens by decreasing frequency.

Reads tokens from stdin (one per line). Sorts them by decreasing frequency as
primary key, with lexicographic ordering of strings as secondary key.
Writes sorted tokens to stdout (one per line).

"""

from __future__ import unicode_literals

from utils import utf8


def main(unused_argv):
  token_count = {}
  for line in utf8.stdin:
    token = line.rstrip('\n')
    token_count[token] = token_count.get(token, 0) + 1
  items = [(-count, token) for token, count in token_count.items()]
  for negative_count, token in sorted(items):
    utf8.Print('%s\t%d' % (token, -negative_count))
  return


if __name__ == '__main__':
  main(None)

#! /usr/bin/env python
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
"""Converts a prompts file in TSV format to Festival's Scheme format.
"""

from __future__ import unicode_literals

import io

STDIN = io.open(0, mode='rt', encoding='utf-8', closefd=False)
STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)


def main(unused_args):
  for line in STDIN:
    line = line.rstrip('\n')
    fields = line.split('\t')
    assert len(fields) >= 2
    utterance_id = fields[0]
    prompt_text = fields[1]
    assert '"' not in prompt_text
    STDOUT.write('( %s "%s" )\n' % (utterance_id, prompt_text))
  return


if __name__ == '__main__':
  main(None)

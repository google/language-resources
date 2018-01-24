# Copyright 2018 Google LLC. All Rights Reserved.
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

"""Writes the PanPhon IPA inventory as UTF-8 text to stdout."""

import io

from absl import app
from fonbund import panphon_util


def main(unused_argv):
  stdout = io.open(1, mode='wt', encoding='utf-8', closefd=False)
  for row in panphon_util.IpaSegments():
    stdout.write('%s\t%s\n' % (row[0], ' '.join(row[1:])))
  stdout.flush()
  return


if __name__ == '__main__':
  app.run(main)

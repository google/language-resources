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

"""Writes segment inventories as UTF-8 text to stdout.
"""

import io
import sys

from absl import app
from absl import flags
from fonbund import segments

STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)
STDERR = io.open(2, mode='wt', encoding='utf-8', closefd=False)

FLAGS = flags.FLAGS

flags.DEFINE_string('db', '', 'Segment inventory, e.g. "panphon", "phoible"')


def main(unused_argv):
  if not FLAGS.db:
    app.usage(shorthelp=True, exitcode=2)
  if FLAGS.db not in segments.TABLES:
    STDERR.write('--db="%s" not recognized; possible values are %s\n'
                 % (FLAGS.db, ', '.join('"%s"' % t for t in segments.TABLES)))
    sys.exit(2)

  for row in segments.SelectFrom(FLAGS.db):
    STDOUT.write('%s\t%s\n' % (row[0], ' '.join(row[1:])))
  STDOUT.flush()
  return


if __name__ == '__main__':
  app.run(main)
